#ifndef UP_FILEREADER_H
#define UP_FILEREADER_H

struct UP_textHandler
{
	char * text;
	int length;
};

long int filesize(const char * filename);
struct UP_textHandler up_loadShaderFile(const char * filename);
void up_textHandler_free(struct UP_textHandler *text_handler);
//char *up_loadImgFile(const char * filename);

#endif