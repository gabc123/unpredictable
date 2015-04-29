//
//  up_assets.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __testprojectshader__up_assets__
#define __testprojectshader__up_assets__
#include "up_vertex.h"
#include "up_texture_module.h"
#include <stdio.h>

#define MODELLENGTH 100

struct up_assets
{
    unsigned int numobjects;
    struct up_mesh *meshArray;
    struct up_texture_data *textureArray;
    struct up_vec3 *scale;
};

struct up_assets *up_assets_start_setup();

#endif /* defined(__testprojectshader__up_assets__) */
