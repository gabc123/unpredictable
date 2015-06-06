#ifndef UP_MATRIXTRANSFORMS_H
#define UP_MATRIXTRANSFORMS_H

#include "up_vertex.h"

//distance between a and b in 3d space
float up_distance(struct up_vec3 a,struct up_vec3 b);

struct up_matrix4
{
    float data[16];
};
typedef struct up_matrix4 up_matrix4_t;



// his structure will hold the diffrent matrix transforms that are needed to
// go from model space to screen space,
// the model matrix is used for lighting , and need to be sent to the gpu, along with mvp
struct up_transformationContainer
{
    up_matrix4_t mvp; //this is the modelviewperspectiv transformation matrix
    up_matrix4_t model; //this is the model transformation matrix, (translate the model coordinates to world coordinates)
    
};

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

void up_normalize(struct up_vec3 *result, struct up_vec3 *vec3A);

struct up_vec3 up_set_vec3(float x, float y, float z);

/*
    this function returns the model matrix transform for the given position , rotation and scaling 
 */
void up_matrixModel(up_matrix4_t *modelMatrix, struct up_vec3 *pos,struct up_vec3 *rotation,struct up_vec3 *scale);
void up_matrixView(up_matrix4_t *matrixView, struct up_vec3 *eye, struct up_vec3 *center,struct up_vec3 *UP);

void up_matrixPerspective(up_matrix4_t *perspective, GLdouble fov,GLdouble aspectRatio,GLdouble zNear,GLdouble zFar);


void up_getViewPerspective(up_matrix4_t *vp,
                           up_matrix4_t *matrixView,
                           up_matrix4_t *perspective);

void up_getModelViewPerspective(up_matrix4_t *mvp,
                                up_matrix4_t *modelMatrix,
                                up_matrix4_t *matrixView,
                                up_matrix4_t *perspective);

#endif //UP_MATRIXTRANSFORMS_H
