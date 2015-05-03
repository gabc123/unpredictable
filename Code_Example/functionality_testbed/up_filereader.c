#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "up_filereader.h"
#include "up_error.h"

/*
	This function will open a file and check its size
	Inpute: filename (file that the size should be checked on)

*/
long int filesize(const char * filename)
{
	long int size = 0;

	// we open the file att the end,
	FILE *fp = fopen(filename,"a");
	// the file pointer is at the end, read the position
    if (fp == NULL) {
        UP_ERROR_MSG_STR("fopen failed, file:",filename);
        return 0;
    }
	size = ftell(fp);
    printf("\nfile loader: end: %ld",size);
	//rewind the file so fp
	rewind(fp);
	size = size - ftell(fp);
    printf("file loader: start - end: %ld\n",size);
	fclose(fp);
	return size;
}


struct UP_textHandler up_loadShaderFile(const char * filename)
{
    struct UP_textHandler zero = {.text = NULL,.length = 0};
    
	long int size = filesize(filename) + 1;
    if (size == 1) {
        //size= 0;
        UP_ERROR_MSG_INT("bad file size:",0);
        return zero;
    }
    
    char *data = malloc(sizeof(char)*size);
    if(data == NULL)
    {
        UP_ERROR_MSG("malloc failure");
        return zero;
    }
    
    FILE *fp = fopen(filename,"r");
    if (fp == NULL) {
        UP_ERROR_MSG_STR("fopen failed, file:",filename);
        
        return zero;
    }

	fread(data,size,sizeof(char),fp);
	data[size - 1] = '\0';
	struct UP_textHandler text;

	text.text = data;
	text.length = (int)size;
	//printf("Shader code for: %s and lenght: %d\n %s",filename,text.length,text.text);
	return text;
}


struct UP_textHandler up_loadObjFile(const char * filename)
{
    return up_loadShaderFile(filename);
}

void up_textHandler_free(struct UP_textHandler *text_handler)
{
    text_handler->length = 0;
    free(text_handler->text);
}


struct UP_textHandler up_loadAssetFile(const char * filename){
    return up_loadShaderFile(filename);
}
