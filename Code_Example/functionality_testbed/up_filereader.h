#ifndef UP_FILEREADER_H
#define UP_FILEREADER_H

/*
    structure that keeps data from a text file and its length
 */
struct UP_textHandler
{
	char * text;
	int length;
};

long int filesize(const char * filename);

/*
    loads diffrent files, and filles out the up_texthandler structure
    All calles need a matching call to up_textHandler_free
 */
struct UP_textHandler up_loadAssetFile(const char * filename);


struct UP_textHandler up_loadShaderFile(const char * filename);

struct UP_textHandler up_loadObjFile(const char * filename);



struct UP_textHandler up_loadWeaponStatsFile(const char * filename);

/*
    up_textHandler_free: free the memory and the file
 */

void up_textHandler_free(struct UP_textHandler *text_handler);
//char *up_loadImgFile(const char * filename);

#endif
