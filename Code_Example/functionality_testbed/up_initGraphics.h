#ifndef UP_INITGRAPHICS_H
#define UP_INITGRAPHICS_H


void UP_sdlSetup();
void UP_openGLwindowSetup(int width,int height, const char *title);
void UP_openGLupdate();
void UP_openGLwindowCleanup();
void UP_sdlCleanup();
void UP_renderBackground();

#endif