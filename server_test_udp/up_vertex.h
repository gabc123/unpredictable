#ifndef UP_VERTEX_H
#define UP_VERTEX_H


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

float up_distance(struct up_vec3 a,struct up_vec3 b);

#endif
