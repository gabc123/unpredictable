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
    struct up_vec2 texCoord;
};

#define MESH_BUFFER_VB  0
#define MESH_BUFFER_TXB  1
#define MESH_BUFFER_INDEX  2
#define MESH_BUFFER_COUNT  3

struct up_mesh
{
	GLuint vertexArrayObj;
	// for vertex, normals, text coord
	GLuint vertexArrayBuffer[MESH_BUFFER_COUNT];
	int vertex_count;
    int index_count;
	int active;
};

void up_mesh_start_setup(int maximum_meshes);
void up_mesh_shutdown_deinit();

struct up_mesh *UP_mesh_new(struct up_vertex *vertex, int vertex_count,unsigned int *indexArray,int index_count);
void up_draw_mesh(struct up_mesh *mesh);

#endif
