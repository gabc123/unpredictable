#include "up_shader_module.h"
#include "filereader/up_filereader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// have all the data nessesary for a complete shader  program
static struct shader_module shader_program;

static GLuint shaderCreate(const char * filename,GLenum type);

void Opengl_error_check(GLuint shader,GLuint flag,int isProgram,const char* str);

struct shader_module *UP_Shader_new(const char * filename)
{
	char vertexFilename[20];
	char fragmentFilename[20];
	strcpy(vertexFilename,filename);
	strcpy(fragmentFilename,filename);

	strncat(vertexFilename,".vs",4);
	strncat(fragmentFilename,".fs",4);
	
	//GLint shader[2];
	shader_program.program = glCreateProgram();
	shader_program.shader[0] = shaderCreate(vertexFilename,GL_VERTEX_SHADER);
	shader_program.shader[1] = shaderCreate(fragmentFilename,GL_FRAGMENT_SHADER);

	glAttachShader(shader_program.program, shader_program.shader[0]);
	glAttachShader(shader_program.program, shader_program.shader[1]);

	glBindAttribLocation(shader_program.program,0,"position");

	glLinkProgram(shader_program.program);
    Opengl_error_check(shader_program.program,GL_LINK_STATUS,1,"link error : ");

	glValidateProgram(shader_program.program);
        Opengl_error_check(shader_program.program,GL_LINK_STATUS,1,"Validate, invalid shader program: ");
	return &shader_program;
}


static GLuint shaderCreate(const char * filename,GLenum type)
{
	struct UP_textHandler shaderSourcetext;

	GLuint shader = glCreateShader(type);

	const GLchar * sourceText[1];
	GLint sourceLength[1];

	shaderSourcetext = up_loadShaderFile(filename);
	sourceText[0] = shaderSourcetext.text;
	sourceLength[0] = shaderSourcetext.length;

	glShaderSource(shader,1,sourceText,sourceLength);
	glCompileShader(shader);
    
    Opengl_error_check(shader,GL_COMPILE_STATUS,0,"ERROR, compiling shader program: ");
    up_textHandler_free(&shaderSourcetext);
    return shader;
}


void UP_shader_bind(struct shader_module *prog)
{
	glUseProgram(prog->program);
}

void UP_Shader_delete()
{
	glDetachShader(shader_program.program,shader_program.shader[0]);
	glDeleteShader(shader_program.shader[0]);


	glDetachShader(shader_program.program,shader_program.shader[1]);
	glDeleteShader(shader_program.shader[1]);

	glDeleteProgram(shader_program.program);

}

static void Opengl_error_program_check(GLuint shader,GLuint flag,const char* str)
{
    GLint success = 0;
    GLchar errormsg[1024] = {0};
    glGetProgramiv(shader, flag, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(shader, sizeof(errormsg), NULL, errormsg);
        fprintf(stderr, "OpenGl program error , msg: %s log: %s \n",str,errormsg);
    }
}

static void Opengl_error_shader_check(GLuint shader,GLuint flag,const char* str)
{
    GLint success = 0;
    GLchar errormsg[1024] = {0};
    glGetShaderiv(shader, flag, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(errormsg), NULL, errormsg);
        fprintf(stderr, "OpenGl program error , msg: %s log: %s \n",str,errormsg);
    }
}

void Opengl_error_check(GLuint shader,GLuint flag,int isProgram,const char* str)
{
   
    
    if (isProgram) {
        Opengl_error_program_check(shader,flag,str);
    }else
    {
        Opengl_error_shader_check(shader, flag, str);
    }
    
}






