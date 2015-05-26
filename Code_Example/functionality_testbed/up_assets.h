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
#include "up_shader_module.h"

#define MODELLENGTH 100


struct up_assets
{
    unsigned int numobjects;
    struct up_mesh *meshArray;
    struct up_texture_data *textureArray;
    struct up_vec3 *scaleArray;
    struct up_vec3 *hitboxLengthArray;
};

#define UP_NUMBER_LETTER 106

struct up_font_assets
{
    struct up_texture_data *texture;
    int size;
    struct up_mesh letters[UP_NUMBER_LETTER];

};

struct up_objectInfo up_asset_createObjFromId(int modelId);
struct up_assets *up_assets_start_setup();
void up_assets_shutdown_deinit(struct up_assets *assets);


struct up_font_assets * up_font_start_setup();
void up_displayText(char *text_string,int length,struct up_vec3 *pos,
                    struct up_vec3 *scale,struct up_font_assets *fonts,
                    struct shader_module *shaderprog, float step,struct up_vec3 *color);

#endif /* defined(__testprojectshader__up_assets__) */
