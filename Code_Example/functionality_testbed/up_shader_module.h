#ifndef UP_SHADER_MODULE_H
#define UP_SHADER_MODULE_H

#include "up_opengl_redirect.h"
#include "up_matrixTransforms.h"

#define UP_SHADER_MAX_COUNT 10

#define UNIFORM_TRANSFORM 0
#define UNIFORM_LIGHT_SUN 1
// ambient light uniforms
#define UNIFORM_AMBIENT_COLOR 2
#define UNIFORM_AMBIENT_INTENSITY 3

#define UNIFORM_MODEL_WORLD 4
// directional light
#define UNIFORM_DIRECTIONAL_LIGHT_COLOR 5
#define UNIFORM_DIRECTIONAL_LIGHT_DIR 6
#define UNIFORM_DIRECTIONAL_LIGHT_INTENSITY 7
#define UNIFORM_SIZE 8

/*
 uniform vec3 ambiantColor;
 uniform float ambiantIntensity;
 
 uniform vec3 lightColor;
 uniform vec3 lightDir;
 uniform float lightIntensity
 
 
 */

struct shader_module
{
	GLuint program;
	GLuint shader[2];
    GLuint uniforms[UNIFORM_SIZE];
};

struct shader_module *UP_Shader_new(const char * filename,int location);

void UP_shader_bind(struct shader_module *prog);

void UP_shader_update(struct shader_module *prog,up_matrix4_t *transform);

void up_shader_update_modelWorld(struct shader_module *prog,up_matrix4_t *modelWorld);

void up_shader_update_font_color(struct shader_module *prog,struct up_vec3 *color);

// light
void up_shader_update_ambient(struct shader_module *prog,struct up_vec3 *color,float *intensity);

void up_shader_update_directional_light(struct shader_module *prog,struct up_vec3 *color,float *intensity,struct up_vec3 *dir);



void up_shader_update_sunligth(struct shader_module *prog,up_matrix4_t *transform);

void UP_Shader_delete();

#endif