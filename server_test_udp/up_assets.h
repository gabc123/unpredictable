//
//  up_assets.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __testprojectshader__up_assets__
#define __testprojectshader__up_assets__
#include "up_vertex.h"

#define MODELLENGTH 100


struct up_assets
{
    unsigned int numobjects;
    struct up_vec3 *scaleArray;
    struct up_vec3 *hitboxLengthArray;
};


struct up_objectInfo up_asset_createObjFromId(int modelId);
struct up_assets *up_assets_start_setup();
void up_assets_shutdown_deinit(struct up_assets *assets);


#endif /* defined(__testprojectshader__up_assets__) */
