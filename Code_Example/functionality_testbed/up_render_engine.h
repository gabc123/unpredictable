//
//  up_render_engine.h
//  up_game
//
//  Created by Waleed Hassan on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#ifndef UP_RENDER_ENGINE_H
#define UP_RENDER_ENGINE_H

#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"
#include "up_shader_module.h"
#include "up_assets.h"

typedef struct up_skyBox{
    
    struct up_texture_data *textureId;
    struct up_mesh *mesh;
    struct shader_module *skyBox;
        
}up_skyBox_t;


void up_render_scene(struct up_transformationContainer *modelViewPerspectiveArray,struct up_objectInfo *objectArray,int count,
                     up_matrix4_t *viewPerspectivMatrix,struct shader_module *shaderprog,struct up_assets *assets,up_skyBox_t *skyBox);

#endif /* defined(__up_game__up_render_engine__) */
