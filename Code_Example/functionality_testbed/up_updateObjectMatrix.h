#ifndef UP_UPDATEOBJECTMATRIX_H
#define UP_UPDATEOBJECTMATRIX_H

#include "up_math.h"

#include "up_object_handler.h"



void up_updateMatrix(struct up_transformationContainer *modelReturnData, up_matrix4_t *viewPerspectivMatrix, struct up_objectInfo *objectArray, int count);


#endif //UP_UPDATEOBJECTMATRIX_H

