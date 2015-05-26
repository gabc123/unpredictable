//
//  up_skyBox.c
//  up_game
//
//  Created by Waleed Hassan on 2015-05-11.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#include "up_skyBox.h"


/*
 up_matrixModel(&modelReturnData[i].model,&objectArray[i].pos, &rotation, &objectArray[i].scale);
 
 //up_getModelViewPerspective(&modelReturnData[i], &modelMatrix, viewMatrix, perspectiveMatrix);
 
 up_matrix4Multiply(&modelReturnData[i].mvp, &modelReturnData[i].model, viewPerspectivMatrix);
 
 */

void up_skybox_render(struct up_skyBox *skyBox,struct up_camera *cam,up_matrix4_t *viewPerspectivMatrix)
{
    UP_shader_bind(skyBox->skyBox);
    
    // skybox is the backround therefor its always going to be farest away. we dont need any skybuffer.inspiration
    GLint previousFaceValue;
    glGetIntegerv(GL_CULL_FACE_MODE, &previousFaceValue);
    GLint previousDepthValue;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthValue);
    
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    
    up_matrix4_t model;
    up_matrix4_t mvp;
    struct up_vec3 rot = {0};
    struct up_vec3 scale = {1.0,1.0,1.0};
    up_matrixModel(&model,&cam->eye,&rot,&scale);
    up_matrix4Multiply(&mvp,&model,viewPerspectivMatrix);
    
    UP_shader_update(skyBox->skyBox, &mvp);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox->textureId->textureId);
    
    up_draw_mesh(skyBox->mesh);
    glCullFace(previousFaceValue);
    glDepthFunc(previousDepthValue);
}

