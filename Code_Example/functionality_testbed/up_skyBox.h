//
//  up_skyBox.h
//  up_game
//
//  Created by Waleed Hassan on 2015-05-11.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#ifndef UP_SKYBOX_H
#define UP_SKYBOX_H

#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"
#include "up_shader_module.h"
#include "up_assets.h"
#include "up_camera_module.h"

struct up_skyBox {
    
    struct up_texture_data *textureId;
    struct up_mesh *mesh;
    struct shader_module *skyBox;
    
};

void up_skybox_render(struct up_skyBox *skyBox,struct up_camera *cam,up_matrix4_t *viewPerspectivMatrix);
#endif /* defined(__up_game__up_skyBox__) */
