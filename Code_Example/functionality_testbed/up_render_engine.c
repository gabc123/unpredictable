//
//  up_render_engine.c
//  up_game
//
//  Created by Waleed & Magnus on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#include "up_render_engine.h"
#include "up_initGraphics.h"
#include "up_texture_module.h"

void up_render_scene(struct up_transformationContainer *modelViewPerspectiveArray,struct up_objectInfo *objectArray,int count,
                     struct shader_module *shaderprog,struct up_assets *assets)
{

    struct up_mesh *mesh = NULL;//&assets->meshArray[1];
    struct up_texture_data *texture = NULL;// &assets->textureArray[1];
    up_matrix4_t *transform = NULL;
    up_matrix4_t *model = NULL;
    //up_matrix4_t identity = up_matrix4identity();
    struct up_vec3 ambientColor = {0.4,0.4,0.4};
    float ambientIntensity = 0.05;

    struct up_vec3 lightColor = {0.4,0.4,0.4};
    float lightIntensity = 0.01;
    struct up_vec3 lightdir = {0};
    UP_renderBackground();                      //Clears the buffer and results an empty window.
    UP_shader_bind(shaderprog);                 // tells the gpu what shader program to use

    int i = 0;
    int modelId = 0;
    for (i = 0; i < count; i++) {

        modelId = objectArray[i].modelId;
        if (modelId >= assets->numobjects) {
            modelId = 0;
        }
        texture = &assets->textureArray[modelId];
        mesh = &assets->meshArray[modelId];

        transform = &modelViewPerspectiveArray[i].mvp;
        model = &modelViewPerspectiveArray[i].model;
        up_texture_bind(texture, 0);

        UP_shader_update(shaderprog,transform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh

        up_shader_update_modelWorld(shaderprog, model);

        up_shader_update_ambient(shaderprog, &ambientColor,&ambientIntensity);

        lightdir = objectArray[i].pos;
        lightdir.z -=80;
        up_shader_update_directional_light(shaderprog, &lightColor, &lightIntensity, &objectArray[i].pos);

        //up_shader_update_sunligth(shaderprog, &identity);    // this uploads the sun angle to the gpu



        up_draw_mesh(mesh); // this draws the model onto the screen , at the location transform, and with the sunlight modelMatrix

    }

    UP_openGLupdate();  // this swaps the render and window buffer , displaying it on screen
}
