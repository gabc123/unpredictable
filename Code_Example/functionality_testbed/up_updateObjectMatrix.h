#ifndef UP_UPDATEOBJECTMATRIX_H
#define UP_UPDATEOBJECTMATRIX_H

#include "up_vertex.h"

#include "up_modelRepresentation.h"
#include "up_ship.h"


void up_updateMatrix(struct up_transformationContainer *modelReturnData, up_matrix4_t *viewMatrix ,up_matrix4_t *perspectiveMatrix, struct up_objectInfo *objectArray, int count);


#endif //UP_UPDATEOBJECTMATRIX_H

