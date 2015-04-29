#ifndef UP_UPDATEOBJECTMATRIX_H
#define UP_UPDATEOBJECTMATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include "up_vertex.h"
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"
#include "up_ship.h"




void updateMatrix(up_matrix4_t *modelReturnData, up_matrix4_t *viewMatrix ,up_matrix4_t *perspectiveMatrix, struct up_objectInfo *objectArray, int count)
{
    up_matrix4_t matrixModel;
    up_matrix4_t modelMatrix;
    struct up_vec3 rotation;
    int i;

    for(i=0;i<count;i++)
    {
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = objectArray[i].angle;

        up_matrixModel(&matrixModel,&objectArray[i].pos, &rotation, &objectArray[i].scale);

        up_getModelViewPerspective(&modelReturnData[i], &modelMatrix, viewMatrix, perspectiveMatrix);

    }



}



#endif //UP_UPDATEOBJECTMATRIX_H
