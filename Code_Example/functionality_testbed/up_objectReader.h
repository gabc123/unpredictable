#ifndef UP_OBJECTREADER_H
#define UP_OBJECTREADER_H

#include "up_vertex.h"


struct up_objModel{
	unsigned int *indexArray; //index of vertex order, what vertex will be used in this polygon.
	struct up_vec3 *vertex;
};

struct up_objModel *up_loadObjModel(const char *filename);

#endif //UP_OBJECTREADER_H
