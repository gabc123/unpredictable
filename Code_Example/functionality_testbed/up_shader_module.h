#ifndef UP_SHADER_MODULE_H
#define UP_SHADER_MODULE_H

#include "up_opengl_redirect.h"
#include "up_matrixTransforms.h"

#define UP_SHADER_MAX_COUNT 10

#define UNIFORM_TRANSFORM 0
#define UNIFORM_LIGHT_SUN 1
#define UNIFORM_SIZE 2

struct shader_module
{
	GLuint program;
	GLuint shader[2];
    GLuint uniforms[UNIFORM_SIZE];
};

struct shader_module *UP_Shader_new(const char * filename,int location);

void UP_shader_bind(struct shader_module *prog);
void UP_shader_update(struct shader_module *prog,up_matrix4_t *transform);
void up_shader_update_sunligth(struct shader_module *prog,up_matrix4_t *transform);

void UP_Shader_delete();

#endif