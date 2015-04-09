#ifndef UP_SHADER_MODULE_H
#define UP_SHADER_MODULE_H

#include "up_opengl_redirect.h"

struct shader_module
{
	GLuint program;
	GLuint shader[2];
};

struct shader_module *UP_Shader_new(const char * filename);

void UP_shader_bind(struct shader_module *prog);

void UP_Shader_delete();

#endif