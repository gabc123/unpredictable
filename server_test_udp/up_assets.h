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

/*
    All models have a hit box and a scale, this is the structure that contains them
    where the index in de array equals the model number, (model nr is the order they are listed in objIndex file)
 */
struct up_assets
{
    unsigned int numobjects;
    struct up_vec3 *scaleArray;
    struct up_vec3 *hitboxLengthArray;
};

/*
    up_asset_createObjFromId:
    takes the model the object should be created from
    Returns a object with all nessecary information,
    this function is the primary way of creating objects in the game,
    It makes sure that all data in the object is correct and works
 */
struct up_objectInfo up_asset_createObjFromId(int modelId);

/*
    up_assets_start_setup:
    starts this module/subsystem, loads all object and assets in game, and keeps track of them
    it returns a pointer to accsess them.
    needs to be match by a call to up_assets_shutdown_deinit when program is shutdown
 */
struct up_assets *up_assets_start_setup();

/*
    shutsdown the module/subsytem and free all memory
 */
void up_assets_shutdown_deinit(struct up_assets *assets);


#endif /* defined(__testprojectshader__up_assets__) */
