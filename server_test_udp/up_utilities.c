//
//  util.c
//
//  Created by o_0 on 2015-04-19.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "up_error.h"
#include <time.h>

unsigned int up_clock_ms()
{
    return (double)clock()/(CLOCKS_PER_SEC/1000);
}

/*
 this is the srand and rand function from:
 ISO/IEC 9899:TC3 Committee Draft — Septermber 7, 2007 WG14/N1256
 7.20.2 Pseudo-random sequence generation functions
 page 312
 we needed a platform independet srand and rand.
 mac and linux seemed to have different implementations so we used the one from the standard
 */
static unsigned long int next = 1;
void up_srand(unsigned int seed)   // from the standard
{
    next = seed;
}

int up_rand(void)
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}



/*
 Description:
 this will scan the srcString untill it find delimiter,
 if found then it will replace the char with \0
 and return the start address, and also set nextString to the next seq
 if not found then it will return NULL

 Parameters:
 char *srcString : NULL terminated string that will be tokenized (is destructiv) (need to be on heap)
 char **nextString : Will be set to the next part of the string after the first token
 const char *delimiter: NULL terminated string that will be scanded for
 int delimiter_length : how many char delimiter is .
 Return:
 returns a pointer to the start of the token string, it will be set to NULL if failure
 (nextString will be repointed at the next string in case of sucsess)

 inspired-by C library function - strtok(),
 */
char *up_token_parser(char *srcString,char **nextString,char *delimiter,unsigned long delimiter_length)
{
    if (srcString == NULL) {
        fprintf(stderr, "Error passed src == NULL to token_parser ");
    }
    if (delimiter == NULL) {
        fprintf(stderr, "Error passed delimiter == NULL to token_parser ");
    }
    char *start = srcString;
    char *ch = srcString;
    int i = 0;
    //printf("The string : %s \n",ch);
    while (ch[i] != '\0') {
        if (ch[i] == *delimiter) {
            if(strncmp(&ch[i], delimiter, delimiter_length) != 0) {
                //did not match whole delimiter so continue loop
                i++;
                continue;
            }
            // found the delimiter we wanted
            // replace it with '\0'
            ch[i] = '\0';
            // to hopp over the delimiter
            i = i + (int)delimiter_length;
            // now  set the next string and return the the token
            *nextString = &ch[i];
            return start;
        }
        i++;
    }
    return NULL;
}




//////////////////////////////////////////////////////

/*****************************************************************
 Generic list, with dynamic memory handling , this is inspired-by c++ STL vector .
 This generic list is not thread safe
 Following functions are provided:
 new: creates a new list and preforms setup to store data in it,
 delete: frees all the data in the list and the list,
 transferOwnership: this will return a pointer to a array of size count with the data,
 this operation will destroy the list making it unusable, this means that you will have to remember to free the memory yourself.

 add: add a given element to the end of the list
 set: sets new value for the element at index in list, (index < count)
 get: get the value at the element index and fills it in data (index < count)

 count: return the current size of the list

 *****************************************************************/

struct up_generic_list
{
    void *data;
    unsigned int element_size;
    unsigned int count;
    unsigned int capacity;
};

#define UP_GENERIC_LIST_RESIZE_FACTOR 1.6


// obj lifetime functions
static struct up_generic_list *up_generic_list_new(unsigned int element_size,unsigned int start_capacity);

static void up_generic_list_delete(struct up_generic_list *list);

static void *up_generic_list_transferOwnership(struct up_generic_list **list);

// information

static unsigned int up_generic_list_count(struct up_generic_list *list);

// add,set,get
static int up_generic_list_add(struct up_generic_list *list,void *data);

static void up_generic_list_getElement(struct up_generic_list *list,void *data,unsigned int atIndex);

static void up_generic_list_setElement(struct up_generic_list *list,void *data,unsigned int atIndex);

////////////////
//helper functions
static int generic_list_resize(struct up_generic_list *list);
static void generic_copyElement(unsigned int element_size,char *destination,char *source);




/*
 generic list , implementation
 */
static struct up_generic_list *up_generic_list_new(unsigned int element_size,unsigned int start_capacity)
{
    struct up_generic_list *list = malloc(sizeof(struct up_generic_list));
    if (list == NULL) {
        UP_ERROR_MSG("malloc fail list");
        return NULL;
    }
    list->count = 0;
    list->element_size = element_size;

    // So we do not instantly need to realloc new mamory for the first add
    start_capacity = (start_capacity < 4) ? 16 : start_capacity;

    void *data = malloc(element_size * start_capacity);
    if (data == NULL) {
        UP_ERROR_MSG("malloc fail data");
        free(list);
        return NULL;
    }
    list->data = data;
    list->capacity = start_capacity;
    return list;
}

static void up_generic_list_delete(struct up_generic_list *list)
{
    if (list == NULL) {
        UP_ERROR_MSG("Tried to free data that was already freed");
        return;
    }
    list->capacity = 0;
    list->count = 0;
    list->element_size = 0;
    free(list->data);
    free(list);
}

static void *up_generic_list_transferOwnership(struct up_generic_list **list)
{

    struct up_generic_list *list_tmp = *list;
    *list = NULL;
    void *data = list_tmp->data;
    list_tmp->data = NULL;
    list_tmp->capacity = 0;
    list_tmp->count = 0;
    list_tmp->element_size = 0;
    free(list_tmp);

    return data;
}

static unsigned int up_generic_list_count(struct up_generic_list *list)
{
    return list->count;
}

#define UP_GENERIC_LIST_RESIZE_FACTOR 1.6


// this function uses realloc to expand the memory block in the list
// it expands by a factor UP_GENERIC_LIST_RESIZE_FACTOR .
// to prevent memory leak we save the pointer to reset the listpointer in case of failure
static int generic_list_resize(struct up_generic_list *list)
{
    //if realloc fails we dont want to lose track of the memory
    void *tmp_data = list->data;

    unsigned int newCapacity = (unsigned int)((double)list->capacity * UP_GENERIC_LIST_RESIZE_FACTOR);
    void *new_data = realloc(list->data, newCapacity * list->element_size);
    if (new_data == NULL) {
        UP_ERROR_MSG("realloc failed");
        list->data = tmp_data;
        return 0;
    }
    // success repoint the data at correct place
    list->data = new_data;
    list->capacity = newCapacity;
    return 1;
}

// warning:
// this function copy the data from source into destination,
// this function is pure evil and copy raw byte data of element_size,
// thid function do not check if destination and source is pointing to valid data
// and it do not do any bound checking
static void generic_copyElement(unsigned int element_size,char *destination,char *source)
{
    char *end = destination + element_size;
    for (; destination < end; destination++,source++) {
        *destination = *source;
    }
}

static int up_generic_list_add(struct up_generic_list *list,void *data)
{
    //check if we need to reallocate
    if (list->count >= list->capacity) {
        if (generic_list_resize(list) == 0) {
            return 0;
        }
    }

    //evil stuff
    // need to cast to char (byte) to do pointer math
    // void * math is not allowed
    char *base = (char*)list->data;
    // move to the correct location
    base = base + list->element_size * list->count;
    char *source = (char*)data;

    // copy the data into the lists memmory block
    generic_copyElement(list->element_size,base,source);
    list->count++;
    return 1;
}


static void up_generic_list_getElement(struct up_generic_list *list,void *data,unsigned int atIndex)
{
    if (atIndex >= list->capacity) {
        UP_ERROR_MSG("outofbounds access");
        return;
    }
    //evil stuff
    // need to cast to char (byte) to do pointer math
    // void * math is not allowed
    char *base = (char*)list->data;
    // move to the correct location
    base = base + list->element_size * atIndex;
    char *destination = (char*)data;
    // copy the data into the lists memmory block
    generic_copyElement(list->element_size,destination,base);
}

static void up_generic_list_setElement(struct up_generic_list *list,void *data,unsigned int atIndex)
{
    if (list->count >= list->capacity) {
        UP_ERROR_MSG("outofbounds access");
        return;
    }
    //evil stuff
    // need to cast to char (byte) to do pointer math
    // void * math is not allowed
    char *base = (char*)list->data;
    // move to the correct location
    char *destination = base + list->element_size * atIndex;
    char *source = (char*)data;
    // copy the data into the lists memmory block
    generic_copyElement(list->element_size,destination,source);
}

/*****************************************************

    wrapper functions to hide the generic list
    they work by calling the generic version
    and only pass the data along,
    the compiler should be smart enguoh to remove it
 *****************************************************/




/*
 wrapper function for up_vec3 specific types.
 */

// all list functions for vec3
struct up_generic_list *up_vec3_list_new(unsigned int start_capacity)
{
    unsigned int element_size = sizeof(struct up_vec3);
    return up_generic_list_new(element_size,start_capacity);
}

void up_vec3_list_delete(struct up_generic_list *list)
{
    up_generic_list_delete(list);
}

struct up_vec3 *up_vec3_list_transferOwnership(struct up_generic_list **list)
{
    return (struct up_vec3 *)up_generic_list_transferOwnership(list);
}

// information

unsigned int up_vec3_list_count(struct up_generic_list *list)
{
    return up_generic_list_count(list);
}


// add,set,get
int up_vec3_list_add(struct up_generic_list *list,struct up_vec3 *data)
{
    return up_generic_list_add(list,data);
}

void up_vec3_list_getAtIndex(struct up_generic_list *list,struct up_vec3 *data,unsigned int atIndex)
{
    up_generic_list_getElement(list,data,atIndex);
}

void up_vec3_list_setAtIndex(struct up_generic_list *list,struct up_vec3 *data,unsigned int atIndex)
{
    up_generic_list_setElement(list,data,atIndex);
}

/*
 wrapper function for up_vec2 specific types.
 */

struct up_generic_list *up_vec2_list_new(unsigned int start_capacity)
{
    unsigned int element_size = sizeof(struct up_vec2);
    return up_generic_list_new(element_size,start_capacity);
}

void up_vec2_list_delete(struct up_generic_list *list)
{
    up_generic_list_delete(list);
}

struct up_vec2 *up_vec2_list_transferOwnership(struct up_generic_list **list)
{
    return (struct up_vec2 *)up_generic_list_transferOwnership(list);
}

// information

unsigned int up_vec2_list_count(struct up_generic_list *list)
{
    return up_generic_list_count(list);
}


// add,set,get
int up_vec2_list_add(struct up_generic_list *list,struct up_vec2 *data)
{
    return up_generic_list_add(list,data);
}

void up_vec2_list_getAtIndex(struct up_generic_list *list,struct up_vec2 *data,unsigned int atIndex)
{
    up_generic_list_getElement(list,data,atIndex);
}

void up_vec2_list_setAtIndex(struct up_generic_list *list,struct up_vec2 *data,unsigned int atIndex)
{
    up_generic_list_setElement(list,data,atIndex);
}



/*
 wrapper function for up_vertex specific types.
 */

struct up_generic_list *up_vertex_list_new(unsigned int start_capacity)
{
    unsigned int element_size = sizeof(struct up_vertex);
    return up_generic_list_new(element_size,start_capacity);
}

void up_vertex_list_delete(struct up_generic_list *list)
{
    up_generic_list_delete(list);
}

struct up_vertex *up_vertex_list_transferOwnership(struct up_generic_list **list)
{
    return (struct up_vertex *)up_generic_list_transferOwnership(list);
}

// information
unsigned int up_vertex_list_count(struct up_generic_list *list)
{
    return up_generic_list_count(list);
}


// add,set,get
int up_vertex_list_add(struct up_generic_list *list,struct up_vertex *data)
{
    return up_generic_list_add(list,data);
}

void up_vertex_list_getAtIndex(struct up_generic_list *list,struct up_vertex *data,unsigned int atIndex)
{
    up_generic_list_getElement(list,data,atIndex);
}

void up_vertex_list_setAtIndex(struct up_generic_list *list,struct up_vertex *data,unsigned int atIndex)
{
    up_generic_list_setElement(list,data,atIndex);
}


/*
 wrapper function for up_int specific types.
 */

struct up_generic_list *up_int_list_new(unsigned int start_capacity)
{
    unsigned int element_size = sizeof(int);
    return up_generic_list_new(element_size,start_capacity);
}

void up_int_list_delete(struct up_generic_list *list)
{
    up_generic_list_delete(list);
}

int *up_int_list_transferOwnership(struct up_generic_list **list)
{
    return (int *)up_generic_list_transferOwnership(list);
}

// information

unsigned int up_int_list_count(struct up_generic_list *list)
{
    return up_generic_list_count(list);
}


// add,set,get
int up_int_list_add(struct up_generic_list *list,int *data)
{
    return up_generic_list_add(list,data);
}

void up_int_list_getAtIndex(struct up_generic_list *list,int *data,unsigned int atIndex)
{
    up_generic_list_getElement(list,data,atIndex);
}

void up_int_list_setAtIndex(struct up_generic_list *list,int *data,unsigned int atIndex)
{
    up_generic_list_setElement(list,data,atIndex);
}

/*
 wrapper function for unsigned int specific types.
 */

struct up_generic_list *up_uint_list_new(unsigned int start_capacity)
{
    unsigned int element_size = sizeof(unsigned int);
    return up_generic_list_new(element_size,start_capacity);
}

void up_uint_list_delete(struct up_generic_list *list)
{
    up_generic_list_delete(list);
}

unsigned int *up_uint_list_transferOwnership(struct up_generic_list **list)
{
    return (unsigned int *)up_generic_list_transferOwnership(list);
}

// information

unsigned int up_uint_list_count(struct up_generic_list *list)
{
    return up_generic_list_count(list);
}


// add,set,get
int up_uint_list_add(struct up_generic_list *list,unsigned int *data)
{
    return up_generic_list_add(list,data);
}

void up_uint_list_getAtIndex(struct up_generic_list *list,unsigned int *data,unsigned int atIndex)
{
    up_generic_list_getElement(list,data,atIndex);
}

void up_uint_list_setAtIndex(struct up_generic_list *list,unsigned int *data,unsigned int atIndex)
{
    up_generic_list_setElement(list,data,atIndex);
}






