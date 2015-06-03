#ifndef UP_VERTEX_H
#define UP_VERTEX_H
#include "up_opengl_redirect.h"



struct up_vec3
{
	float x,y,z;
};

struct up_vec2
{
	float x,y;
};

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

// Initiate internal structure takes in the maximum number of models that can be loaded onto the gpu
void up_mesh_start_setup(int maximum_meshes);
// call to shutdown the module
void up_mesh_shutdown_deinit();

// creates a new mesh, takes a vertexArray , and a indexArray, and the length of both
// this will then upload the model to the gpu, and give back a structure to use containing the ids to the gpu.
struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count,unsigned int *indexArray,int index_count);
// draw the mesh that has been uploaded to the gpu
void up_draw_mesh(struct up_mesh *mesh);

// this is used to draw a mesh with a diffrent mode, like wireframe
void up_draw_mesh_speciall(struct up_mesh *mesh,int drawMode);

#endif
