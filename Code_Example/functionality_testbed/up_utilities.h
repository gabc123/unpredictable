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


#endif /* UP_UTILITIES_H */
