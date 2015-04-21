#include "up_matrixTransforms.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

up_matrix4_t up_matrix4Xrotation(float angle);
up_matrix4_t up_matrix4Yrotation(float angle);
up_matrix4_t up_matrix4Zrotation(float angle);


up_matrix4_t up_matrix4translation(struct up_vec3 *vec3);
up_matrix4_t up_matrix4scaling(struct up_vec3 *vec3);


void dispMat(up_matrix4_t *mat)
{
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        printf("%f %f %f %f\n", mat->data[i*4 +0],mat->data[i*4 +1],mat->data[i*4 +2],mat->data[i*4 +3]);
    }
}

/*
    Rotation matrix, scaling matrix was based on :
    http://www.gamedev.net/page/resources/_/technical/math-and-physics/matrix-math-r877
 */

up_matrix4_t up_matrix4Xrotation(float angle)
{
    float xsin = sinf(angle);
    float xcos = cosf(angle);
    up_matrix4_t xrot = {1, 0, 0, 0,
        0, xcos, -xsin, 0,
        0, xsin, xcos, 0,
        0, 0, 0, 1};
    return xrot;
}

up_matrix4_t up_matrix4Yrotation(float angle)
{
    float ysin = sinf(angle);
    float ycos = cosf(angle);
    up_matrix4_t yrot = {ycos, 0, ysin, 0,
        0, 1, 0, 0,
        -ysin, 0, ycos, 0,
        0, 0, 0, 1};
    return yrot;
}

up_matrix4_t up_matrix4Zrotation(float angle)
{
    float zsin = sinf(angle);
    float zcos = cosf(angle);
    up_matrix4_t zrot = {zcos, -zsin, 0, 0,
        zsin, zcos, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    return zrot;
}

up_matrix4_t up_matrix4identity()
{
    up_matrix4_t identity ={1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    return identity;
}


up_matrix4_t up_matrix4translation(struct up_vec3 *vec3)
{
    up_matrix4_t translation ={1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        vec3->x, vec3->y, vec3->z, 1};
    return translation;
}

up_matrix4_t up_matrix4scaling(struct up_vec3 *vec3)
{
    up_matrix4_t translation ={vec3->x, 0, 0, 0,
        0, vec3->y, 0, 0,
        0, 0, vec3->z, 0,
        0, 0, 0, 1};
    return translation;
}

/*
	This function takes two 4by4 matrix and multiply them togheter
	Parm:
 result is a pointer to the location the result should be stored
 matA is the first matrix to be multiplyed
 matB is the second matrix to be multiplyed
 */
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

void up_matrixModel(up_matrix4_t *modelMatrix, struct up_vec3 *pos,struct up_vec3 *rotation,struct up_vec3 *scale)
{


    up_matrix4_t mat4scale = up_matrix4scaling(scale);

    up_matrix4_t rotXmatrix4 = up_matrix4Xrotation(rotation->x);
    up_matrix4_t rotYmatrix4 = up_matrix4Yrotation(rotation->y);
    up_matrix4_t rotZmatrix4 = up_matrix4Zrotation(rotation->z);

    up_matrix4_t tmpResult = {0};
    up_matrix4_t rotationMatrix = {0};

    up_matrix4Multiply(&tmpResult, &rotXmatrix4, &rotYmatrix4);
    up_matrix4Multiply(&rotationMatrix, &tmpResult, &rotZmatrix4);

    up_matrix4_t posMatrix4 = up_matrix4translation(pos);

    //up_matrix4_t modelMatrix = {0};

    // To calculate the model transform we need to first calculate
    // the scale then rotation then translation
    // this is becouse rotation is around origo and if we translate the coordinates before
    // then the rotation will look wrong
    up_matrix4Multiply(&tmpResult, &mat4scale, &rotationMatrix);
    up_matrix4Multiply(modelMatrix, &tmpResult, &posMatrix4);
}

void up_cross(struct up_vec3 *result,struct up_vec3 *vec3B,struct up_vec3 *vec3A)
{
    result->x = (vec3A->y*vec3B->z) - (vec3A->z*vec3B->y);
    result->y = -(vec3A->x*vec3B->z - vec3A->z*vec3B->x);
    result->z = vec3A->x*vec3B->y - vec3A->y*vec3B->x;
}


float up_dot(struct up_vec3 *vec3A, struct up_vec3 *vec3B)
{
    return (vec3A->y*vec3B->y + vec3A->z*vec3B->z + vec3A->x*vec3B->x);
}

//takes 2 vector pointers and puts their subtracted result into the result vector pointer
static void viewdirection(struct up_vec3 *result,struct up_vec3 *center,struct up_vec3 *eye){
    result->x = center->x - eye->x;
    result->y = center->y - eye->y;
    result->z = center->z - eye->z;
}

//the mathematic operation normalize
void normalize(struct up_vec3 *result, struct up_vec3 *vec3A)
{
 //   struct up_vec3 tmp;
    float dot=sqrt(up_dot(vec3A,vec3A));
    result->x = vec3A->x/dot;
    result->y = vec3A->y/dot;
    result->z = vec3A->z/dot;
}


// https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
void up_matrixView(up_matrix4_t *matrixView, struct up_vec3 *eye, struct up_vec3 *center,struct up_vec3 *UP)
{
    struct up_vec3 result;
    struct up_vec3 U;
    viewdirection(&result,center,eye);
    struct up_vec3 F=result;
    struct up_vec3 f;
    struct up_vec3 fn;
    struct up_vec3 S;
    struct up_vec3 UPn;
    struct up_vec3 Sn;

    normalize(&f,&F);
    normalize(&fn,&f);
    normalize(&UPn,UP);
    up_cross(&S,&fn,&UPn);
    normalize(&Sn,&S);
    up_cross(&U,&Sn, &fn);


    float matTmp[16] = {S.x,S.y,S.z,0, U.x,U.y,U.z,0, f.x,f.y,f.z,0, 0,0,0,1};
    int  i = 0;
    for (i = 0; i< 16; i++) {
        matrixView->data[i] = matTmp[i];
    }
}

// based on https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
void up_matrixPerspective(up_matrix4_t *perspective, GLdouble fov,GLdouble aspectRatio,GLdouble zNear,GLdouble zFar)
{
    double f = 1/tan(fov*0.5);
    double invFrustrum = 1/(zFar - zNear);
    
    perspective->data[0] = f/aspectRatio;
    perspective->data[5] = f;
    perspective->data[10] = zFar*invFrustrum;
    perspective->data[11] = 1;
    perspective->data[14] = (-zFar*zNear)*invFrustrum;
    //perspective->data[15] = 1;
}

void up_getModelViewPerspective(up_matrix4_t *mvp,
                                up_matrix4_t *modelMatrix,
                                up_matrix4_t *matrixView,
                                up_matrix4_t *perspective)
{
    up_matrix4_t mv = {0};
    up_matrix4Multiply(&mv,modelMatrix,matrixView);
    up_matrix4Multiply(mvp, &mv, perspective);
    
    /*up_matrix4_t pv = {0};
    up_matrix4Multiply(&pv,perspective,matrixView);
    up_matrix4Multiply(mvp, &pv, modelMatrix);*/
}

/*
int main(int argc, char const *argv[])
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

    up_matrix4_t modal = up_matrixModel(&pos, &rot, &scale);

    printf("modal\n");
    dispMat(&modal);
    return 0;
}*/
