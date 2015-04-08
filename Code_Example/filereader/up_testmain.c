#include <stdio.h>
#include <stdlib.h>
#include "up_filereader.h"

int main(int argc, char const *argv[])
{
	struct UP_textHandler text;

	text = up_loadShaderFile("up_filereader.c");
	printf("Filens storlek is : %d \n",text.length);
	printf("%s\n", text.text);
	free(text.text);
	return 0;
}