#include "up_updateObjectMatrix.h"
#include <stdio.h>
#include <stdlib.h>

#include "up_matrixTransforms.h"

void up_updateMatrix(up_matrix4_t *modelReturnData, up_matrix4_t *viewMatrix ,up_matrix4_t *perspectiveMatrix, struct up_objectInfo *objectArray, int count)
{
    up_matrix4_t modelMatrix;
    struct up_vec3 rotation;
    int i;
    
    for(i=0;i<count;i++)
    {
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = objectArray[i].angle;
        
        up_matrixModel(&modelMatrix,&objectArray[i].pos, &rotation, &objectArray[i].scale);
        
        up_getModelViewPerspective(&modelReturnData[i], &modelMatrix, viewMatrix, perspectiveMatrix);
        
    }
    
    
    
}