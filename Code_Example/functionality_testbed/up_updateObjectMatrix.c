#include "up_updateObjectMatrix.h"
#include <stdio.h>
#include <stdlib.h>

#include "up_matrixTransforms.h"

void up_updateMatrix(up_matrix4_t *modelReturnData, up_matrix4_t *viewMatrix ,up_matrix4_t *perspectiveMatrix, struct up_objectInfo *objectArray, int count)
{
    up_matrix4_t modelMatrix;
    
    // only calculate the viewPerspectivMatrix once and use it multiple times
    // this saves 10-20% atleast
    up_matrix4_t viewPerspectivMatrix;
    up_getViewPerspective(&viewPerspectivMatrix,viewMatrix,perspectiveMatrix);
    
    struct up_vec3 rotation;
    int i;
    
    for(i=0;i<count;i++)
    {
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = objectArray[i].angle;
        
        up_matrixModel(&modelMatrix,&objectArray[i].pos, &rotation, &objectArray[i].scale);
        
        //up_getModelViewPerspective(&modelReturnData[i], &modelMatrix, viewMatrix, perspectiveMatrix);
        
        up_matrix4Multiply(&modelReturnData[i], &modelMatrix, &viewPerspectivMatrix);
        
    }
    
    
    
}