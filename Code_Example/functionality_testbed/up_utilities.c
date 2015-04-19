//
//  util.c
//
//  Created by o_0 on 2015-04-19.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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