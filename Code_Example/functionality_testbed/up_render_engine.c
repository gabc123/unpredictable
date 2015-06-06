//
//  up_render_engine.c
//  up_game
//
//  Created by Waleed & Magnus on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "up_render_engine.h"
#include "up_initGraphics.h"
#include "up_texture_module.h"
#include "up_camera_module.h"
#include "up_star_system.h"

// Magnus functions for presentation
#ifdef UP_PRESENTATION_MODE
// ugly code just for demonstration purposes
static int up_wireFrame_active = 0;
static int up_ambientLigth_active = 1;
static int up_directionalLigth_active = 1;
static int up_allLigth_active = 1;

// activate and shut down light and ambient etc
void up_toggle_wireframe()
{
    up_wireFrame_active = !up_wireFrame_active;
}

int up_wireframe_state()
{
    return up_wireFrame_active;
}


void up_toggle_ambientLigth()
{
    up_ambientLigth_active = !up_ambientLigth_active;
}

int up_ambientLigth_state()
{
    return up_ambientLigth_active;
}


void up_toggle_directionalLigth()
{
    up_directionalLigth_active = !up_directionalLigth_active;
}

int up_directionalLigth_state()
{
    return up_directionalLigth_active;
}


void up_toggle_allLigth()
{
    up_allLigth_active = !up_allLigth_active;
}

int up_allLigth_state()
{
    return up_allLigth_active;
}

#endif

// magnus, this renders the scene
void up_render_scene(struct up_transformationContainer *modelViewPerspectiveArray,struct up_objectInfo *objectArray,int count,
                     up_matrix4_t *viewPerspectivMatrix,struct shader_module *shaderprog,struct up_assets *assets)
{

    struct up_mesh *mesh = NULL;//&assets->meshArray[1];
    struct up_texture_data *texture = NULL;// &assets->textureArray[1];
    up_matrix4_t *transform = NULL;
    up_matrix4_t *model = NULL;
    //up_matrix4_t identity = up_matrix4identity();

    struct up_vec3 ambientColor = {0.4,0.4,0.4};
    float ambientIntensity = 1.0;

    struct up_vec3 lightColor = {0.2,0.2,0.2};
    float lightIntensity = 0.01;
    struct up_vec3 lightdir = {0};

#ifdef UP_PRESENTATION_MODE
    float ambientIntensity_original = ambientIntensity;
    struct up_vec3 ambientColor_original = ambientColor;
    struct up_vec3 whiteColor = {1.0,1.0,1.0};
    float zeroIntensity = 0.0f;

    float lightIntensity_original = lightIntensity;
#endif

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

#ifdef UP_PRESENTATION_MODE
        if (up_ambientLigth_state()) {
            ambientIntensity = ambientIntensity_original;
        }else
        {
            ambientIntensity = zeroIntensity;
        }

        if (!up_allLigth_state()) {
            ambientColor = whiteColor;
            ambientIntensity = 1.0;
        }else
        {
            ambientColor = ambientColor_original;
        }

        // directional light settings
        if (up_directionalLigth_state()) {
            lightIntensity = lightIntensity_original;
        }else
        {
            lightIntensity = zeroIntensity;
        }

#endif

        up_shader_update_ambient(shaderprog, &ambientColor,&ambientIntensity);

        lightdir = objectArray[i].pos;
        lightdir.z = 80;
        up_shader_update_directional_light(shaderprog, &lightColor, &lightIntensity, &objectArray[i].pos);


#ifdef UP_PRESENTATION_MODE
        if (!up_wireframe_state()) {
            up_draw_mesh(mesh); // this draws the model onto the screen , at the location transform, and with the sunlight modelMatrix
        }else
        {
            up_draw_mesh_speciall(mesh, GL_LINES);
        }
#else
        up_draw_mesh(mesh);
#endif


    }

     // this swaps the render and window buffer , displaying it on screen
}








void up_render_sun(struct up_sun *sunData,struct up_camera *cam ,up_matrix4_t *viewPerspectivMatrix,struct up_assets *assets )
{
    struct up_render_metaData *sun = &sunData->render;
    
    int modelId = sun->modelId;
    if (modelId >= assets->numobjects) {
        modelId = 0;
    }
    
    float none = 0.0; // not used
    // get the camera vector
    struct up_vec3 tmpDir = {0};
    struct up_vec3 cam_normalized = {0};
    
    tmpDir.x = cam->center.x - cam->eye.x;
    tmpDir.y = cam->center.y - cam->eye.y;
    tmpDir.z = cam->center.z - cam->eye.z;
    up_normalize(&cam_normalized, &tmpDir);
    
    // load models
    struct up_mesh *mesh = &assets->meshArray[modelId];
    struct up_texture_data *texture = &assets->textureArray[modelId];
    
    // the diffrent shells of the sun
    struct up_sun_shell *innerShell = &sunData->shells[0];
    struct up_sun_shell *outerShell = &sunData->shells[1];
    
    
    UP_shader_bind(sun->shader);
    up_texture_bind(texture, 0);
    
    // creates the mvp transform for this shell
    up_matrix4Multiply(&sun->transform, &innerShell->modelTransforms, viewPerspectivMatrix);
    
    // transformation matrox. mvp and m
    UP_shader_update(sun->shader, &sun->transform);
    up_shader_update_modelWorld(sun->shader, &innerShell->modelTransforms);

    // how the sun fragment shader shpuld draw the shells
    up_shader_update_ambient(sun->shader, &innerShell->sunColor,&innerShell->dropOff_dir);
    up_shader_update_directional_light(sun->shader, &innerShell->ligthDropoff, &none, &cam_normalized);
    
    up_draw_mesh(mesh);
    
    
    // creates the mvp transform for this shell
    up_matrix4Multiply(&sun->transform, &outerShell->modelTransforms, viewPerspectivMatrix);
    
    // transformation matrox. mvp and m
    UP_shader_update(sun->shader, &sun->transform);
    up_shader_update_modelWorld(sun->shader, &outerShell->modelTransforms);
    
    // how the sun fragment shader shpuld draw the shells
    up_shader_update_ambient(sun->shader, &outerShell->sunColor,&outerShell->dropOff_dir);
    up_shader_update_directional_light(sun->shader, &outerShell->ligthDropoff, &none, &cam_normalized);
    
    up_draw_mesh(mesh);
}
