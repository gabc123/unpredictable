#ifndef UP_FILEREADER_H
#define UP_FILEREADER_H

struct UP_textHandler
{
	char * text;
	int length;
};
struct UP_textHandler up_loadAssetFile(const char * filename);
long int filesize(const char * filename);
struct UP_textHandler up_loadShaderFile(const char * filename);
struct UP_textHandler up_loadObjFile(const char * filename);
void up_textHandler_free(struct UP_textHandler *text_handler);
struct UP_textHandler up_loadWeaponStatsFile(const char * filename);
//char *up_loadImgFile(const char * filename);

#endif
