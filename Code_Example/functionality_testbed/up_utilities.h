//
//  util.h
//
//  Created by o_0 on 2015-04-19.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_UTILITIES_H
#define UP_UTILITIES_H

#include "up_vertex.h"

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
 
 *****************************************************************/

// this is implemented in the c file, we do not want to give access to the internal structure
struct up_generic_list;

/*
    To prevent type mismatch when using the generic list , we will hide it behinde wrapper functions
    for most types
 */

/*
    wrapper function for up_vec3 specific types.
 */

// all list functions for vec3

struct up_generic_list *up_vec3_list_new(unsigned int start_capacity);
void up_vec3_list_delete(struct up_generic_list *list);
struct up_vec3 *up_vec3_list_transferOwnership(struct up_generic_list **list);

// information

unsigned int up_vec3_list_count(struct up_generic_list *list);


// add,set,get
int up_vec3_list_add(struct up_generic_list *list,struct up_vec3 *data);

void up_vec3_list_getAtIndex(struct up_generic_list *list,struct up_vec3 *data,unsigned int atIndex);
void up_vec3_list_setAtIndex(struct up_generic_list *list,struct up_vec3 *data,unsigned int atIndex);

#endif /* UP_UTILITIES_H */
