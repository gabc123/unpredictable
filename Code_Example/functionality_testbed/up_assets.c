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
#include "filereader/up_filereader.h"
#include "up_sdl_redirect.h"

void loadObjects(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_generic_list *scaleArray);

struct up_modelData
{
    char obj[MODELLENGTH];
    char tex[MODELLENGTH];
    struct up_vec3 scale;
};

struct up_mesh *dummyobj()
{
    struct up_mesh *mesh = meshTriangleShip();
    return mesh;
}


int up_process_asset(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_modelData *item)
{
    int returnCode = 1;
    struct up_objModel *testObj = NULL;
    struct up_mesh *mesh = NULL;
    struct up_texture_data *texture = NULL;
    
    // set up the dummyobjects to be used incase of failure
    struct up_mesh dummyMesh;
    struct up_texture_data dummyTexture;
    up_mesh_list_getAtIndex(meshArray, &dummyMesh, 0);
    up_texture_list_getAtIndex(textureArray, &dummyTexture, 0);
    
    //struct load item;
    testObj= up_loadObjModel(item->obj);
    if (testObj !=NULL) {
        mesh = UP_mesh_new(testObj->vertex, testObj->vertex_length, testObj->indexArray, testObj->index_length);
        up_objModelFree(testObj);
    }else{
        mesh = NULL;
    }
    
    if (mesh == NULL) {
        mesh = &dummyMesh;//reuse the same dummy mesh, reducing gpu memory presure
        returnCode = 0;
    }
    up_mesh_list_add(meshArray, mesh);
    
    texture = up_load_texture(item->tex);

    if (texture == NULL) {
        texture=&dummyTexture; //reuse the same dummy texture, reducing gpu memory presure
        returnCode = 0;
    }

    up_texture_list_add(textureArray, texture);
    return returnCode;
}

void loadObjects(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_generic_list *scaleArray)
{
    
    struct UP_textHandler thafile = up_loadAssetFile("objIndex");
    struct up_vec3 scaleOne = {1.0, 1.0, 1.0};
    struct up_modelData item; //stores
    char *text = thafile.text;
    char *endLine = "\n";
    char *rad;;
    
    //    struct up_vec3 scale;
    
    /*reads from the file and stores read data*/
    do{
        rad = up_token_parser(text, &text, endLine, strlen(endLine));
        if(rad == NULL)
        {
            UP_ERROR_MSG("ERROR, obj String could not be found in loadObjects");
            break;
        }
        sscanf(rad,"%f %f %f %s %s", &item.scale.x, &item.scale.y, &item.scale.z, item.obj, item.tex);
        if(up_process_asset(meshArray,textureArray,&item) == 0)
        {
            item.scale = scaleOne; // there has been a error , set scale to one
        }
        
        up_vec3_list_add(scaleArray, &item.scale);
        
    }while(text <= thafile.text + thafile.length -1);
    
    up_textHandler_free(&thafile);
    
    
}


struct up_assets *up_assets_start_setup()
{

    struct up_generic_list *meshArray = up_mesh_list_new(10);
    struct up_generic_list *textureArray = up_texture_list_new(10);
    struct up_generic_list *scaleArray= up_vec3_list_new(10);
    
    // the first model is always a default model  that is used if
    // a missing obj file or texture is found
    struct up_mesh *mesh = dummyobj();
    struct up_texture_data *texture = up_load_texture("lala.png");
    if ((mesh == NULL) || (texture == NULL)) {
        UP_ERROR_MSG("Major problem , default texture or mesh failed to load, expect segfaults incoming");
    }
    struct up_vec3 scale = {1.0, 1.0, 1.0};
    up_mesh_list_add(meshArray, mesh);
    up_texture_list_add(textureArray, texture);
    up_vec3_list_add(scaleArray, &scale);
    

    loadObjects(meshArray, textureArray, scaleArray);

    struct up_assets *assets = malloc(sizeof(struct up_assets));
    if (assets == NULL) {
        UP_ERROR_MSG("failure in assets");
    }
    assets->numobjects = up_mesh_list_count(meshArray);
    
    assets->meshArray = up_mesh_list_transferOwnership(&meshArray);
    assets->textureArray = up_texture_list_transferOwnership(&textureArray);
    assets->scaleArray =up_vec3_list_transferOwnership(&scaleArray);
    
    return assets;
}

void up_assets_shutdown_deinit(struct up_assets *assets)
{
    free(assets->meshArray);
    free(assets->textureArray);
    free(assets->scaleArray);
    free(assets);
}

/*
    loadobject loads models listed in the objIndex file.
*/

