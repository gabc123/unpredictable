#ifndef UP_OBJECTREADER_H
#define UP_OBJECTREADER_H

#include "vertex.h"


struct up_objModel{
	unsigned int *indexArray[]; //index of vertex order, what vertex will be used in this polygon.
	struct up_vec3 *vertex;
};

#endif //UP_OBJECTREADER_H
