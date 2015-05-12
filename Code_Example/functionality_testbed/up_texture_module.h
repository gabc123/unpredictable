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


int up_texture_start_setup();
void up_texture_shutdown_deinit();


struct up_texture_data *up_load_texture(const char  * filename);
struct up_texture_data *up_loadImage_withAlpha(const char  * filename);


void up_texture_bind(struct up_texture_data *texture, unsigned int texUnit);
struct up_texture_data *up_cubeMapTexture_load();


#endif /* defined(UP_TEXTURE_MODULE_H) */
