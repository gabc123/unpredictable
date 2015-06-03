#ifndef UP_OBJECTREADER_H
#define UP_OBJECTREADER_H

#include "up_vertex.h"


struct up_objModel {
    int index_length;
	unsigned int *indexArray; //index of vertex order, what vertex will be used in this polygon.
	int vertex_length;
    struct up_vertex *vertex;
};


/*
reads a .obj file and

Returns a list with usable position values for vertexes.
Returns an index array
*/
struct up_objModel *up_loadObjModel(const char *filename);


void up_objModelFree(struct up_objModel *object);

#endif //UP_OBJECTREADER_H
