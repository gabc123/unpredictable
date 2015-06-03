//
//  up_texture_module.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_TEXTURE_MODULE_H
#define UP_TEXTURE_MODULE_H
#include "up_opengl_redirect.h"

struct up_texture_data
{
    GLuint textureId;
};

// setsup the texture module
int up_texture_start_setup();
void up_texture_shutdown_deinit();


// this loads a texture, from the game folder, with the filename
// if there also is a file with the same name but ends in Alpha.png, it loads the
// red channel as alpha and adds it to the texture
struct up_texture_data *up_load_texture(const char  * filename);


// this tells the gpu what texture to use
void up_texture_bind(struct up_texture_data *texture, unsigned int texUnit);

// Load function for the cube map , used for the skybox
struct up_texture_data *up_cubeMapTexture_load();


#endif /* defined(UP_TEXTURE_MODULE_H) */
