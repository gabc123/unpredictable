#ifndef UP_OBJECTREADER_H
#define UP_OBJECTREADER_H

#include "up_vertex.h"


struct up_objModel{
    int index_length;
	unsigned int *indexArray; //index of vertex order, what vertex will be used in this polygon.
	int vertex_length;
    struct up_vertex *vertex;
};

struct up_objModel *up_loadObjModel(const char *filename);

#endif //UP_OBJECTREADER_H
