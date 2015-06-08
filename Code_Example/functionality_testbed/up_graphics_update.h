//
//  up_graphics_update.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef unpredictable_xcode_up_graphics_update_h
#define unpredictable_xcode_up_graphics_update_h
#include "up_graphics_setup.h"
#include "up_assets.h"

/////// implemented in up_texture_module.c

// this tells the gpu what texture to use
void up_texture_bind(struct up_texture *texture, unsigned int texUnit);



/////// implemented in up_shader_module.c
// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html
//https://youtu.be/0jML5fMBDGg?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh

// binds the shader to be used for rendering
void up_shader_bind(struct up_shader_module *prog);

// uploads the transform  matrix to the gpu
void up_shader_update(struct up_shader_module *prog,up_matrix4_t *transform);

// uploads diffrent informatino to the gpu for the model to be renderd.
void up_shader_update_modelWorld(struct up_shader_module *prog,up_matrix4_t *modelWorld);

void up_shader_update_font_color(struct up_shader_module *prog,struct up_vec3 *color);

// light
void up_shader_update_ambient(struct up_shader_module *prog,struct up_vec3 *color,float *intensity);

void up_shader_update_directional_light(struct up_shader_module *prog,struct up_vec3 *color,float *intensity,struct up_vec3 *dir);

void up_shader_update_sunligth(struct up_shader_module *prog,up_matrix4_t *transform);

//////// implemented in up_mesh.c
// draw the mesh that has been uploaded to the gpu
void up_mesh_draw(struct up_mesh *mesh);

// this is used to draw a mesh with a diffrent mode, like wireframe
void up_mesh_draw_speciall(struct up_mesh *mesh,int drawMode);


#endif
