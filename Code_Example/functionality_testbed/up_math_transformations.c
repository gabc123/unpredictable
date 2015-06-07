//
//  up_math_transformations.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_math.h"
#include <math.h>

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


//takes 2 vector pointers and puts their subtracted result into the result vector pointer
//Sebastian
static void viewdirection(struct up_vec3 *result,struct up_vec3 *center,struct up_vec3 *eye){
    result->x = center->x - eye->x;
    result->y = center->y - eye->y;
    result->z = center->z - eye->z;
}

/*
 this function takes the position , rotation and scale , that should be applied to a model
 it fills out the modelMatrix with the correct values to perform this transformation
 between the model coordinates to the world coordinates
 */
// magnus
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

/*
 this creates the view matrix that transform the world coord to the screen coords
 */
//magnus
void up_viewTest2(up_matrix4_t *matrixView, struct up_vec3 *eye, struct up_vec3 *center,struct up_vec3 *UP)
{
    struct up_vec3 dir;
    viewdirection(&dir, center, eye);
    struct up_vec3 face;
    up_normalize(&face, &dir);
    
    //normalized upvector
    struct up_vec3 un;
    up_normalize(&un, UP);
    
    struct up_vec3 cross_fu;
    up_cross(&cross_fu, &face, &un);
    
    struct up_vec3 shader;
    up_normalize(&shader, &cross_fu);
    
    struct up_vec3 upp;
    up_cross(&upp, &shader, &face);
    
    float row4x = -up_dot(&shader, eye);
    float row4y = -up_dot(&upp, eye);
    float row4z = -up_dot(&face, eye);
    
    up_matrix4_t matTmp = {
        shader.x, upp.x, face.x, 0,
        shader.y, upp.y, face.y, 0,
        shader.z, upp.z, face.z, 0,
        row4x, row4y, row4z,1};
    
    *matrixView = matTmp;
    
    //struct up_vec3 invEye = {eye->x,eye->y,eye->z};
    //up_matrix4_t camTranslation = up_matrix4translation(&invEye);
    
    //up_matrix4Multiply(matrixView, &camTranslation, &matTmp);
    
}



// https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
//Sebastian
void up_matrixView(up_matrix4_t *matrixView, struct up_vec3 *eye, struct up_vec3 *center,struct up_vec3 *UP)
{
    up_viewTest2(matrixView,eye,center,UP);
   
}

// based on https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
void up_matrixPerspective(up_matrix4_t *perspective, double fov,double aspectRatio,double zNear,double zFar)
{
    up_matrix4_t identity=up_matrix4identity();
    double f = 1/tan(fov*0.5);
    double invFrustrum = 1/(zFar - zNear);
    
    *perspective=identity;
    
    perspective->data[0] = f/aspectRatio;
    perspective->data[5] = f;
    perspective->data[10] = zFar*invFrustrum;
    perspective->data[11] = 1;
    perspective->data[14] = (-zFar*zNear)*invFrustrum;
    //perspective->data[15] = 1;
}

// this returns the viewPerspective matrix,
// becouse the view and perspective is constant during the entire frame
// we only need to get this one time for eech frame, reducing the cpu load.
void up_getViewPerspective(up_matrix4_t *vp,
                           up_matrix4_t *matrixView,
                           up_matrix4_t *perspective)
{
    up_matrix4Multiply(vp, matrixView, perspective);
    
}


// This function takes the mode, view and perspective matrix, and returns the
// final transformations matrix that is used by the gpu
void up_getModelViewPerspective(up_matrix4_t *mvp,
                                up_matrix4_t *modelMatrix,
                                up_matrix4_t *matrixView,
                                up_matrix4_t *perspective)
{
    up_matrix4_t mv = {0};
    up_matrix4Multiply(&mv,modelMatrix,matrixView);
    up_matrix4Multiply(mvp, &mv, perspective);
    
}