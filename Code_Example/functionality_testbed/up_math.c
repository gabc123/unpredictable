#include "up_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// created by magnus
// modifyed by sebastian
// modifyed by magnus


up_matrix4_t up_matrix4Xrotation(float angle);
up_matrix4_t up_matrix4Yrotation(float angle);
up_matrix4_t up_matrix4Zrotation(float angle);


up_matrix4_t up_matrix4translation(struct up_vec3 *vec3);
up_matrix4_t up_matrix4scaling(struct up_vec3 *vec3);

// display the matrix in the console, used for math debugging
void dispMat(up_matrix4_t *mat)
{
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        printf("%f %f %f %f\n", mat->data[i*4 +0],mat->data[i*4 +1],mat->data[i*4 +2],mat->data[i*4 +3]);
    }
}



/*
	This function takes two 4by4 matrix and multiply them togheter
	Parm:
 result is a pointer to the location the result should be stored
 matA is the first matrix to be multiplyed
 matB is the second matrix to be multiplyed
 */
// magnus
void up_matrix4Multiply(up_matrix4_t *result, up_matrix4_t *matA, up_matrix4_t *matB)
{
    float *A = &(matA->data[0]);
    float *B = &(matB->data[0]);
    int i = 0;
    // Top row of the 4by4 matrix
    for (i = 0; i < 4; i++)
    {
        result->data[i] = A[0]*B[0 + i] + A[1]*B[4 + i] + A[2]*B[8 + i] + A[3]*B[12 + i];
    }

    // Top middle row of the 4by4 matrix
    for (i = 0; i < 4; i++)
    {
        result->data[4 + i] = A[4]*B[0 + i] + A[5]*B[4 + i] + A[6]*B[8 + i] + A[7]*B[12 + i];
    }

    //  Bottom middle of the 4by4 matrix
    for (i = 0; i < 4; i++)
    {
        result->data[8 + i] = A[8]*B[0 + i] + A[9]*B[4 + i] + A[10]*B[8 + i] + A[11]*B[12 + i];
    }

    // Bottom row of the 4by4 matrix
    for (i = 0; i < 4; i++)
    {
        result->data[12 + i] = A[12]*B[0 + i] + A[13]*B[4 + i] + A[14]*B[8 + i] + A[15]*B[12 + i];
    }

}


//matrix cross operation
//Sebastian
void up_cross(struct up_vec3 *result,struct up_vec3 *vec3A,struct up_vec3 *vec3B)
{
    result->x = vec3A->y * vec3B->z - vec3A->z * vec3B->y;
    result->y = vec3A->z * vec3B->x - vec3A->x * vec3B->z;
    result->z = vec3A->x * vec3B->y - vec3A->y * vec3B->x;
}

//Matrix dot operation
//Sebastian
float up_dot(struct up_vec3 *vec3A, struct up_vec3 *vec3B)
{
    return (vec3A->y*vec3B->y + vec3A->z*vec3B->z + vec3A->x*vec3B->x);
}


//the mathematic operation normalize
//Sebastian
void up_normalize(struct up_vec3 *result, struct up_vec3 *vec3A)
{
 //   struct up_vec3 tmp;
    float dot=sqrtf(up_dot(vec3A,vec3A));
    result->x = vec3A->x/dot;
    result->y = vec3A->y/dot;
    result->z = vec3A->z/dot;
}

struct up_vec3 up_set_vec3(float x, float y, float z){
    
    struct up_vec3 vec = {x,y,z};
    
    return vec;
}


float up_distance(struct up_vec3 a,struct up_vec3 b)
{
    float dx = a.x -b.x;
    float dy = a.y -b.y;
    float dz = a.z -b.z;
    
    return sqrtf(dx*dx + dy*dy + dz*dz);
}
