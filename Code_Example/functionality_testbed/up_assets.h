//
//  up_assets.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __testprojectshader__up_assets__
#define __testprojectshader__up_assets__
#include "up_opengl_redirect.h"
#include "up_math.h"


/////// implemented in up_texture_module.c

struct up_skyBox {
    
    struct up_texture *textureId;
    struct up_mesh *mesh;
    struct up_shader_module *skyBox;
    
};

struct up_texture
{
    GLuint textureId;
};


/////// implemented in up_shader_module.c

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

#define UP_SHADER_MAX_COUNT 10

struct up_shader_module
{
    GLuint program;
    GLuint shader[2];
    GLuint uniforms[UNIFORM_SIZE];
};

/////////////vertex and mesh , used by up_vertex

struct up_vertex
{
    struct up_vec3 pos;
    struct up_vec3 normals;
    struct up_vec2 texCoord;
};

// based on tutorials from
//https://youtu.be/csKrVBWCItc?list=PLEETnX-uPtBXT9T-hD0Bj31DSnwio-ywh
//http://ogldev.atspace.co.uk/www/tutorial17/tutorial17.html
//http://ogldev.atspace.co.uk/www/tutorial18/tutorial18.html

#define MESH_BUFFER_VB  0
#define MESH_BUFFER_TXB  1
#define MESH_BUFFER_NORM 2
#define MESH_BUFFER_INDEX  3
#define MESH_BUFFER_COUNT  4

struct up_mesh
{
    GLuint vertexArrayObj;
    // for vertex, normals, text coord
    GLuint vertexArrayBuffer[MESH_BUFFER_COUNT];
    GLuint positionId;
    GLuint textureCoordId;
    GLuint normalId;
    GLuint indicesId;
    int vertex_count;
    int index_count;
    int active;
    int drawMode;
};

////////////
#define UP_NUMBER_LETTER 106

/*
 keep track of all the fonst that have been loaded.
 */
struct up_font_assets
{
    struct up_texture *texture;
    int size;
    struct up_mesh letters[UP_NUMBER_LETTER];
    
};


#define MODELLENGTH 100
//////////////


////// implemented in up_objectReader.c
struct up_objModel {
    int index_length;
    unsigned int *indexArray; //index of vertex order, what vertex will be used in this polygon.
    int vertex_length;
    struct up_vertex *vertex;
};

/*
 reads a .obj file
 Returns a list with usable position values for vertexes.
 Returns an index array
 */
struct up_objModel *up_loadObjModel(const char *filename);

void up_objModelFree(struct up_objModel *object);


//////// implemented in up_assets.c
/*
 All models have a hit box and a scale,mesh for information on how to access the gpu,and textures id, this is the structure that contains them
 where the index in de array equals the model number, (model nr is the order they are listed in objIndex file)
 */
struct up_assets
{
    unsigned int numobjects;
    struct up_mesh *meshArray;
    struct up_texture *textureArray;
    struct up_vec3 *scaleArray;
    struct up_vec3 *hitboxLengthArray;
};



/*
 up_asset_createObjFromId:
 takes the model the object should be created from
 Returns a object with all nessecary information,
 this function is the primary way of creating objects in the game,
 It makes sure that all data in the object is correct and works
 */
struct up_objectInfo up_asset_createObjFromId(int modelId);


/////// implemented in up_texture_module.c

// this loads a texture, from the game folder, with the filename
// if there also is a file with the same name but ends in Alpha.png, it loads the
// red channel as alpha and adds it to the texture
struct up_texture *up_load_texture(const char  * filename);

// Load function for the cube map , used for the skybox
struct up_texture *up_cubeMapTexture_load();


/////// implemented in up_shader_module.c
// creates a new shader, from file, and locate it on location
// this will load both .fs and vs, shaders (fragment and vertex)
struct up_shader_module *up_shader_new(const char * filename,int location);




/////// implemented in up_assets.c
struct up_eventState;
void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent);


/////// implemented in up_mesh.c
// creates a new mesh, takes a vertexArray , and a indexArray, and the length of both
// this will then upload the model to the gpu, and give back a structure to use containing the ids to the gpu.
struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count,unsigned int *indexArray,int index_count);

#endif /* defined(__testprojectshader__up_assets__) */
