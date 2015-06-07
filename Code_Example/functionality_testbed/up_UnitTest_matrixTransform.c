
#include "up_math.h"
#include "up_assets.h"
#include "up_graphics_setup.h"
#include "up_graphics_update.h"

#include <stdio.h>
/*
int unittest_matrixtransform(int argc, char const *argv[])
{
    up_matrix4_t A = {1,2,3,4,2,2,2,2,4,4,4,4,6,7,8,9};
    up_matrix4_t B = {1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6};
    up_matrix4_t result = {0};
    
    printf("hello\n");
    dispMat(&A);
    printf("\n");
    printf("\n");
    dispMat(&B);
    printf("\n");
    printf("After multi\n");
    up_matrix4Multiply(&result, &A, &B);
    dispMat(&result);
    
    struct up_vec3 pos = {1, 1, 1};
    struct up_vec3 rot = {0, 1, 1};
    struct up_vec3 scale = {1, 1, 1};
    
    //up_matrix4_t modal = up_matrixModel(&pos, &rot, &scale);
    
    printf("modal\n");
    //dispMat(&modal);
    return 0;
}*/