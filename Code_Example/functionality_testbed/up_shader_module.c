#include "up_shader_module.h"
#include "up_filereader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "up_error.h"

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html
//https://youtu.be/0jML5fMBDGg?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh


// have all the data nessesary for a complete shader  program
static struct shader_module shader_program[UP_SHADER_MAX_COUNT];


static GLuint shaderCreate(const char * filename,GLenum type,int *err);

void Opengl_error_check(GLuint shader,GLuint flag,int isProgram,const char* str);
static void Opengl_error_shader_check(GLuint shader,GLuint flag,const char* str);
static void Opengl_error_program_check(GLuint shader,GLuint flag,const char* str);

struct shader_module *UP_Shader_new(const char * filename,int location)
{
	char vertexFilename[20];
	char fragmentFilename[20];
	strcpy(vertexFilename,filename);
	strcpy(fragmentFilename,filename);

	strncat(vertexFilename,".vs",4);
	strncat(fragmentFilename,".fs",4);

    int errFlag = 0;
	//GLint shader[2];
	shader_program[location].program = glCreateProgram();
	shader_program[location].shader[0] = shaderCreate(vertexFilename,GL_VERTEX_SHADER, &errFlag);
	shader_program[location].shader[1] = shaderCreate(fragmentFilename,GL_FRAGMENT_SHADER, &errFlag);

    if (errFlag == 1) {
        // this happens if shader failed to load,
        UP_ERROR_MSG("faild to create shaders");
        return NULL;
    }
    
	glAttachShader(shader_program[location].program, shader_program[location].shader[0]);
	glAttachShader(shader_program[location].program, shader_program[location].shader[1]);

	glBindAttribLocation(shader_program[location].program,0,"position");
   	glBindAttribLocation(shader_program[location].program,1,"texCoord");
   	glBindAttribLocation(shader_program[location].program,2,"normals");

	glLinkProgram(shader_program[location].program);
    Opengl_error_program_check(shader_program[location].program,GL_LINK_STATUS,"link error : ");

	glValidateProgram(shader_program[location].program);
    Opengl_error_program_check(shader_program[location].program,GL_LINK_STATUS,"Validate, invalid shader program: ");

    shader_program[location].uniforms[UNIFORM_TRANSFORM] = glGetUniformLocation(shader_program[location].program, "mvp");
    shader_program[location].uniforms[UNIFORM_LIGHT_SUN] = glGetUniformLocation(shader_program[location].program, "light_sun");
    // model transform
    shader_program[location].uniforms[UNIFORM_MODEL_WORLD] = glGetUniformLocation(shader_program[location].program, "model");


    // ambient light uniforms
    shader_program[location].uniforms[UNIFORM_AMBIENT_COLOR] = glGetUniformLocation(shader_program[location].program, "ambiantColor");
    shader_program[location].uniforms[UNIFORM_AMBIENT_INTENSITY] = glGetUniformLocation(shader_program[location].program, "ambiantIntensity");

    // directional ligth
    shader_program[location].uniforms[UNIFORM_DIRECTIONAL_LIGHT_COLOR] = glGetUniformLocation(shader_program[location].program, "lightColor");
    shader_program[location].uniforms[UNIFORM_DIRECTIONAL_LIGHT_DIR] = glGetUniformLocation(shader_program[location].program, "lightDir");
    shader_program[location].uniforms[UNIFORM_DIRECTIONAL_LIGHT_INTENSITY] = glGetUniformLocation(shader_program[location].program, "lightIntensity");

	return &shader_program[location];
}



//UNIFORM_LIGHT_SUN
static GLuint shaderCreate(const char * filename,GLenum type,int *err)
{
	struct UP_textHandler shaderSourcetext;

	GLuint shader = glCreateShader(type);

	const GLchar * sourceText[1];
	GLint sourceLength[1];

	shaderSourcetext = up_loadShaderFile(filename);
    if (shaderSourcetext.text == NULL) {
        UP_ERROR_MSG("Failed to load shader file, aborting shader loading");
        *err = 1;
        return 0;
    }
	sourceText[0] = shaderSourcetext.text;
	sourceLength[0] = shaderSourcetext.length;

    fprintf(stderr,"\n\nShader: %s compiling...\n",filename);
	glShaderSource(shader,1,sourceText,sourceLength);
	glCompileShader(shader);

    Opengl_error_shader_check(shader,GL_COMPILE_STATUS,"ERROR, compiling shader program: ");
    up_textHandler_free(&shaderSourcetext);
    return shader;
}


void UP_shader_bind(struct shader_module *prog)
{
	glUseProgram(prog->program);
}

// this will bind the up_matrix transform to the right shader location
// this is here for historic ressons, and will be depricated soono
void UP_shader_update(struct shader_module *prog,up_matrix4_t *transform)
{
    glUniformMatrix4fv(prog->uniforms[UNIFORM_TRANSFORM], 1, GL_FALSE, transform->data);
}

// this is to send the MVP or a generic transformation matric to modify model location data
void up_shader_update_transform(struct shader_module *prog,up_matrix4_t *transform)
{
    glUniformMatrix4fv(prog->uniforms[UNIFORM_TRANSFORM], 1, GL_FALSE, transform->data);
}


void up_shader_update_modelWorld(struct shader_module *prog,up_matrix4_t *modelWorld)
{
    glUniformMatrix4fv(prog->uniforms[UNIFORM_MODEL_WORLD], 1, GL_FALSE, modelWorld->data);
}

void up_shader_update_ambient(struct shader_module *prog,struct up_vec3 *color,float *intensity)
{
    //float localColor[3] = {color.x , color.y , color.z};
    glUniform3fv(prog->uniforms[UNIFORM_AMBIENT_COLOR], 1, (float *)color);
    glUniform1fv(prog->uniforms[UNIFORM_AMBIENT_INTENSITY], 1, intensity);
}

/*
 #define UNIFORM_MODEL_WORLD 4
 // directional light
 #define UNIFORM_DIRECTIONAL_LIGHT_COLOR 5
 #define UNIFORM_DIRECTIONAL_LIGHT_DIR 6
 #define UNIFORM_DIRECTIONAL_LIGHT_INTENSITY 7

 */

void up_shader_update_font_color(struct shader_module *prog,struct up_vec3 *color)
{
    //float localColor[3] = {color.x , color.y , color.z};
    glUniform3fv(prog->uniforms[UNIFORM_AMBIENT_COLOR], 1, (float *)color);
}

void up_shader_update_directional_light(struct shader_module *prog,struct up_vec3 *color,float *intensity,struct up_vec3 *dir)
{

    glUniform3fv(prog->uniforms[UNIFORM_DIRECTIONAL_LIGHT_COLOR], 1, (float *)color);
    glUniform1fv(prog->uniforms[UNIFORM_DIRECTIONAL_LIGHT_INTENSITY], 1, intensity);
    glUniform3fv(prog->uniforms[UNIFORM_DIRECTIONAL_LIGHT_DIR], 1, (float *)dir);
}


void up_shader_update_sunligth(struct shader_module *prog,up_matrix4_t *transform)
{
    glUniformMatrix4fv(prog->uniforms[UNIFORM_LIGHT_SUN], 1, GL_FALSE, transform->data);
}

void UP_Shader_delete()
{
    int i = 0;
    for (i = 0; i< UP_SHADER_MAX_COUNT; i++) {
        glDetachShader(shader_program[i].program,shader_program[i].shader[0]);
        glDeleteShader(shader_program[i].shader[0]);


        glDetachShader(shader_program[i].program,shader_program[i].shader[1]);
        glDeleteShader(shader_program[i].shader[1]);

        glDeleteProgram(shader_program[i].program);
    }


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
