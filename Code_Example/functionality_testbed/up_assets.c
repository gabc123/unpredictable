//
//  up_assets.c
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_assets.h"
#include "up_vertex.h"
#include "up_texture_module.h"
#include "up_utilities.h"
#include "up_objectReader.h"
#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"
#include "testmodels.h"

struct up_mesh *dummyobj()
{
    struct up_mesh *mesh = meshTriangleShip();
    return mesh;
}

struct up_assets *up_assets_start_setup()
{
    
    struct up_objModel *testObj = NULL;
    struct up_mesh *mesh = NULL;
    struct up_texture_data *texture = NULL;
    
    struct up_generic_list *meshArray = up_mesh_list_new(10);
    //struct up_generic_list *textureArray = up_mesh_list_new(10);
    /////////////////////////////
    // TODO: make this into a loop, and readin the models that needs to be loaded from a file
    
    testObj= up_loadObjModel("fighter.obj");
    mesh = UP_mesh_new(testObj->vertex, testObj->vertex_length, testObj->indexArray, testObj->index_length);
    if (mesh == NULL) {
        mesh = dummyobj();
    }
    up_mesh_list_add(meshArray, mesh);
    
    up_objModelFree(testObj);
    texture = up_load_texture("fighter.png");
    if (texture == NULL) {
        up_load_texture("lala.png");
    }
    
    struct up_assets *assets = malloc(sizeof(struct up_assets));
    assets->meshArray = up_mesh_list_transferOwnership(&meshArray);
    assets->textureArray = NULL; // not implemented yet
    
    return assets;
}


