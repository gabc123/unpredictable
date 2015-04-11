#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filereader/up_filereader.h"
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
	long int size = filesize(filename) + 1;
	char *data = malloc(sizeof(char)*size);
	FILE *fp = fopen(filename,"r");

	fread(data,size,sizeof(char),fp);
	data[size - 1] = '\0';
	struct UP_textHandler text;

	text.text = data;
	text.length = (int)size;
	printf("Shader code for: %s and lenght: %d\n %s",filename,text.length,text.text);
	return text;
}


void up_textHandler_free(struct UP_textHandler *text_handler)
{
    text_handler->length = 0;
    free(text_handler->text);
}



