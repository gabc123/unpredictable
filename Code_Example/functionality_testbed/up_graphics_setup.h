#ifndef UP_INITGRAPHICS_H
#define UP_INITGRAPHICS_H
#include "up_assets.h"

void UP_sdlSetup();

void UP_openGLwindowSetup(int width,int height, const char *title);
void UP_openGLupdate();
void UP_openGLwindowCleanup();
void UP_sdlCleanup();
void UP_renderBackground();


//// up_font_module.c
/*
 loads all assets for fonts, all letters and fills out the structure above
 */
struct up_font_assets * up_font_start_setup();


//// up_texture_module.c
// setsup the texture module
int up_texture_start_setup();
void up_texture_shutdown_deinit();

//// up_shader_module.c
// starts shaders subsysetem, us first
void up_shader_setup();
// delet all shaders, us last
void up_shader_deinit();


//// up_assets.c
/*
 up_assets_start_setup:
 starts this module/subsystem, loads all object and assets in game, and keeps track of them
 it returns a pointer to accsess them.
 needs to be match by a call to up_assets_shutdown_deinit when program is shutdown
 */
struct up_assets *up_assets_start_setup();

/*
 shutsdown the module/subsytem and free all memory
 */
void up_assets_shutdown_deinit(struct up_assets *assets);

//// up_mesh.c
// Initiate internal structure takes in the maximum number of models that can be loaded onto the gpu
void up_mesh_start_setup(int maximum_meshes);
// call to shutdown the module
void up_mesh_shutdown_deinit();
#endif