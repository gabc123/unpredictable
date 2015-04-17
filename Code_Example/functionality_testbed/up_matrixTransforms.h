#ifndef UP_MATRIXTRANSFORMS_H
#define UP_MATRIXTRANSFORMS_H

#include "up_vertex.h"

struct up_matrix4
{
    float data[16];
};
typedef struct up_matrix4 up_matrix4_t;

up_matrix4_t up_matrix4identity();

/*
	This function takes two 4by4 matrix and multiply them togheter
	Parm:
 result is a pointer to the location the result should be stored
 matA is the first matrix to be multiplyed
 matB is the second matrix to be multiplyed
 */
void up_matrix4Multiply(up_matrix4_t *result, up_matrix4_t *matA, up_matrix4_t *matB);

void dispMat(up_matrix4_t *mat);


/*
    this function returns the model matrix transform for the given position , rotation and scaling 
 */
void up_matrixModel(up_matrix4_t *modelMatrix, struct up_vec3 *pos,struct up_vec3 *rotation,struct up_vec3 *scale);

#endif //UP_MATRIXTRANSFORMS_H
