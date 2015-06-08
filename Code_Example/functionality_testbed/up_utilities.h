//
//  util.h
//
//  Created by o_0 on 2015-04-19.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_UTILITIES_H
#define UP_UTILITIES_H

#include "up_assets.h"

/*
 this is the srand and rand function from:
 ISO/IEC 9899:TC3 Committee Draft — Septermber 7, 2007 WG14/N1256
 7.20.2 Pseudo-random sequence generation functions
 page 312
 we needed a platform independet srand and rand.
 mac and linux seemed to have different implementations so we used the one from the standard
 */
void up_srand(unsigned int seed);   // from the standard
int up_rand(void);

/*
 Function: up_token_parser
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
//magnus
char *up_token_parser(char *srcString,char **nextString,char *delimiter,unsigned long delimiter_length);


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
 
 iteratorAccess: it will return a pointer to the first element, and change count to the current size,
                WARNING!!: any call to add will invalidate the pointer, you must call interatorAccess agin after every call to add
 *****************************************************************/
// magnus
// forward decleration., this is implemented in the c file, we do not want to give access to the internal structure
struct up_generic_list;


// To use list for diffrent types, we provide wrapper functions, this is more safe then to use
// the generic list directly, this is the wrapper functions for primitive types, int and unsigned int
// below there is a macro to autogenerate this, for all structure that begins with up_
///////////////////////////////////////////////
/*
 wrapper function for int specific types.
 */

struct up_generic_list *up_int_list_new(unsigned int start_capacity);
void up_int_list_delete(struct up_generic_list *list);
int *up_int_list_transferOwnership(struct up_generic_list **list);

// information
unsigned int up_int_list_count(struct up_generic_list *list);

// add,set,get
int up_int_list_add(struct up_generic_list *list,int *data);
void up_int_list_getAtIndex(struct up_generic_list *list,int *data,unsigned int atIndex);
void up_int_list_setAtIndex(struct up_generic_list *list,int *data,unsigned int atIndex);

// iterators
int *up_int_list_iteratorAccess(struct up_generic_list *list,int *count);

///////////////////////////////////////////////
/*
 wrapper function for unsiged int specific types.
 */

struct up_generic_list *up_uint_list_new(unsigned int start_capacity);
void up_uint_list_delete(struct up_generic_list *list);
unsigned int *up_uint_list_transferOwnership(struct up_generic_list **list);

// information
unsigned int up_uint_list_count(struct up_generic_list *list);

// add,set,get
int up_uint_list_add(struct up_generic_list *list,unsigned int *data);
void up_uint_list_getAtIndex(struct up_generic_list *list,unsigned int *data,unsigned int atIndex);
void up_uint_list_setAtIndex(struct up_generic_list *list,unsigned int *data,unsigned int atIndex);

// iterators
unsigned int *up_uint_list_iteratorAccess(struct up_generic_list *list,int *count);



/*
    A macro to auto generate function definitions for specific types
    example, UP_LIST_NEW(mesh) will generate the function definition
        struct up_generic_list *up_mesh_list_new(unsigned int start_capacity);
    This is then used in the c file to auto generate a valid function body
 
    The name is the name of the structur excluding the up_ prefix.
    the following macros can be used:
         
        UP_LIST_NEW(name);
        UP_LIST_DELETE(name);
        UP_LIST_TRANSFER_OWNERSHIP(name);
 
        // information
        UP_LIST_COUNT(name);
 
        // add,set,get
        UP_LIST_ADD(name);
        UP_LIST_GET(name);
        UP_LIST_SET(name);
        
        // iterators
        UP_LIST_ITERATORACCESS(name);
 
 */


// too help pased and join names to create the function name
#define UP_JOIN_NAME(var1,var2) up_ ##var1 ##var2
#define UP_STRUCT_NAME(var1) struct up_ ##var1

// joins the name with the funciton type
#define UP_LIST_NAME_NEW(var1) UP_JOIN_NAME(var1,_list_new)
#define UP_LIST_NAME_DELETE(var1) UP_JOIN_NAME(var1,_list_delete)

#define UP_LIST_NAME_TRANSFER(var1) UP_JOIN_NAME(var1,_list_transferOwnership)
#define UP_LIST_NAME_COUNT(var1) UP_JOIN_NAME(var1,_list_count)

#define UP_LIST_NAME_ADD(var1) UP_JOIN_NAME(var1,_list_add)
#define UP_LIST_NAME_GET(var1) UP_JOIN_NAME(var1,_list_getAtIndex)
#define UP_LIST_NAME_SET(var1) UP_JOIN_NAME(var1,_list_setAtIndex)

#define UP_LIST_NAME_ITERATOR(var1) UP_JOIN_NAME(var1,_list_iteratorAccess)

// the final macro to create the function
#define UP_LIST_NEW(name)\
struct up_generic_list *UP_LIST_NAME_NEW(name)(unsigned int start_capacity)

#define UP_LIST_DELETE(name)\
void UP_LIST_NAME_DELETE(name)(struct up_generic_list *list)

#define UP_LIST_TRANSFER_OWNERSHIP(name)\
UP_STRUCT_NAME(name) *UP_LIST_NAME_TRANSFER(name)(struct up_generic_list **list)

// information
#define UP_LIST_COUNT(name)\
unsigned int UP_LIST_NAME_COUNT(name)(struct up_generic_list *list)

// add,set,get
#define UP_LIST_ADD(name)\
int UP_LIST_NAME_ADD(name)(struct up_generic_list *list,UP_STRUCT_NAME(name) *data)

#define UP_LIST_GET(name)\
void UP_LIST_NAME_GET(name)(struct up_generic_list *list,UP_STRUCT_NAME(name) *data,unsigned int atIndex)

#define UP_LIST_SET(name)\
void UP_LIST_NAME_SET(name)(struct up_generic_list *list,UP_STRUCT_NAME(name) *data,unsigned int atIndex)

// iterators
#define UP_LIST_ITERATORACCESS(name)\
UP_STRUCT_NAME(name) * UP_LIST_NAME_ITERATOR(name)(struct up_generic_list *list,int *count)



/*
    To prevent type mismatch when using the generic list , we will hide it behind wrapper functions
    for most types
 */

///////////////////////////////////////////////
/*
    wrapper function for up_vec3 specific types.
 */
UP_LIST_NEW(vec3);
UP_LIST_DELETE(vec3);
UP_LIST_TRANSFER_OWNERSHIP(vec3);
// information
UP_LIST_COUNT(vec3);
// add,set,get
UP_LIST_ADD(vec3);
UP_LIST_GET(vec3);
UP_LIST_SET(vec3);
// iterators
UP_LIST_ITERATORACCESS(vec3);


///////////////////////////////////////////////
/*
 wrapper function for up_vec2 specific types.
 */
UP_LIST_NEW(vec2);
UP_LIST_DELETE(vec2);
UP_LIST_TRANSFER_OWNERSHIP(vec2);
// information
UP_LIST_COUNT(vec2);
// add,set,get
UP_LIST_ADD(vec2);
UP_LIST_GET(vec2);
UP_LIST_SET(vec2);
// iterators
UP_LIST_ITERATORACCESS(vec2);


///////////////////////////////////////////////
/*
 wrapper function for up_vertex specific types.
 */
///////////////////////////////////////////////
UP_LIST_NEW(vertex);
UP_LIST_DELETE(vertex);
UP_LIST_TRANSFER_OWNERSHIP(vertex);
// information
UP_LIST_COUNT(vertex);
// add,set,get
UP_LIST_ADD(vertex);
UP_LIST_GET(vertex);
UP_LIST_SET(vertex);
// iterators
UP_LIST_ITERATORACCESS(vertex);

///////////////////////////////////////////////
/*
 wrapper function for up_mesh specific types.
 */
UP_LIST_NEW(mesh);
UP_LIST_DELETE(mesh);
UP_LIST_TRANSFER_OWNERSHIP(mesh);
// information
UP_LIST_COUNT(mesh);
// add,set,get
UP_LIST_ADD(mesh);
UP_LIST_GET(mesh);
UP_LIST_SET(mesh);
// iterators
UP_LIST_ITERATORACCESS(mesh);

///////////////////////////////////////////////
/*
 wrapper function for up_texture specific types.
 */

UP_LIST_NEW(texture);
UP_LIST_DELETE(texture);
UP_LIST_TRANSFER_OWNERSHIP(texture);
// information
UP_LIST_COUNT(texture);
// add,set,get
UP_LIST_ADD(texture);
UP_LIST_GET(texture);
UP_LIST_SET(texture);
// iterators
UP_LIST_ITERATORACCESS(texture);
///////////////////////////////////////////////


#endif /* UP_UTILITIES_H */
