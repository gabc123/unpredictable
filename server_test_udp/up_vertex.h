#ifndef UP_VERTEX_H
#define UP_VERTEX_H
//#include "up_opengl_redirect.h"

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

#define MESH_BUFFER_VB  0
#define MESH_BUFFER_TXB  1
#define MESH_BUFFER_NORM 2
#define MESH_BUFFER_INDEX  3
#define MESH_BUFFER_COUNT  4


#endif
