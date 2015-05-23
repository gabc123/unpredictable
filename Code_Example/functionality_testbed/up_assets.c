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
#include "up_filereader.h"
#include "up_sdl_redirect.h"
#include "up_modelRepresentation.h"


static struct up_assets *internal_assets=NULL;

static int loadObjects(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_generic_list *scaleArray);

struct box{
    float x1,y1,z1;
    float x2,y2,z2;
};

struct Collisionbox
{
    int numbox;
    struct box *boxarray;

};
/*stores information of the object*/
//Sebastian
struct up_modelData
{
    char obj[MODELLENGTH];
    char tex[MODELLENGTH];
    struct up_vec3 scale;

};

/*fallback mesh*/
//magnus
struct up_mesh *dummyobj()
{
    struct up_mesh *mesh = meshTriangleShip();
    return mesh;
}

/*
struct up_assets
{
    unsigned int numobjects;
    struct up_mesh *meshArray;
    struct up_texture_data *textureArray;
    struct up_vec3 *scaleArray;
};

struct up_objectInfo
{
    int modelId;
    struct up_vec3 scale;
    struct up_vec3 pos;
    struct up_vec3 dir;
    float angle;
    float turnSpeed;
    float speed;
    float acceleration;
};
*/

void fallbackHitbox(struct up_objectInfo *obj){
    //if(obj->collisionbox.xmax < internal_assets[i].meshArray.vertexArrayObj.)
}

/*Returns stored date of the model*/
//Sebastian
struct up_objectInfo up_asset_createObjFromId(int modelId)
{
    struct up_objectInfo obj = {0};
    obj.scale.x = 1;
    obj.scale.y = 1;
    obj.scale.z = 1;
    
    obj.modelId = modelId;

    //internal_assets is a static global
    obj.scale = internal_assets->scaleArray[modelId];

    fallbackHitbox(&obj);

    return obj;
}

//magnus
int up_process_asset(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_modelData *item)
{
    int returnCode = 1, i;
    struct up_objModel *testObj = NULL;
    struct up_mesh *mesh = NULL;
    struct up_texture_data *texture = NULL;

    float xmax=0, ymax=0, zmax=0, xmin=0, ymin=0, zmin=0;

    // set up the dummyobjects to be used in case of failure
    struct up_mesh dummyMesh;
    struct up_texture_data dummyTexture;
    up_mesh_list_getAtIndex(meshArray, &dummyMesh, 0);
    up_texture_list_getAtIndex(textureArray, &dummyTexture, 0);

    //struct load item;
    testObj= up_loadObjModel(item->obj);
    if (testObj != NULL) {
        for(i=0; i< testObj->vertex_length; i++){
            if(xmax<testObj->vertex[i].pos.x)
                xmax=testObj->vertex[i].pos.x;
            if(ymax<testObj->vertex[i].pos.y)
                ymax=testObj->vertex[i].pos.y;
            if(zmax<testObj->vertex[i].pos.z)
                zmax=testObj->vertex[i].pos.z;
            if(xmin>testObj->vertex[i].pos.x)
                xmin=testObj->vertex[i].pos.x;
            if(ymin>testObj->vertex[i].pos.y)
                ymin=testObj->vertex[i].pos.y;
            if(zmin<testObj->vertex[i].pos.z)
                zmin=testObj->vertex[i].pos.z;
        }
    }


    printf("xmax: %f ymax %f zmax %f xmin: %f ymin %f zmin %f\n", xmax,ymax,zmax, xmin,ymin,zmin);

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

/*Reads assets to be loaded from a file*/
//Sebastian
// magnus error handeling
static int loadObjects(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_generic_list *scaleArray)
{
    struct UP_textHandler thafile = up_loadAssetFile("objIndex");
    if (thafile.text == NULL) {
        UP_ERROR_MSG("Failed to load objindex");
        return 0;   // failed ti a load objindex
    }
    
    struct up_vec3 scaleOne = {1.0, 1.0, 1.0};
    struct up_modelData item; //stores the information of the object
    char *text = thafile.text;
    char *endLine = "\n";
    char *row;

    //    struct up_vec3 scale;

    /*reads from the file and stores read data*/
    do{
        row = up_token_parser(text, &text, endLine, strlen(endLine));
        if(row == NULL)
        {
            //UP_ERROR_MSG("ERROR, obj String could not be found in loadObjects");
            printf("end of file objIndex\n");
            break;
        }
        sscanf(row,"%f %f %f %s %s", &item.scale.x, &item.scale.y, &item.scale.z, item.obj, item.tex);
        if(up_process_asset(meshArray,textureArray,&item) == 0)
        {
            item.scale = scaleOne; // there has been a error , set scale to one
        }

        up_vec3_list_add(scaleArray, &item.scale);

    }while(text <= thafile.text + thafile.length -1);

    up_textHandler_free(&thafile);
    return 1; // sucess

}

/*loads all stored assets into the gpu*/
//Sebastian
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
        return NULL;
    }
    assets->numobjects = up_mesh_list_count(meshArray);

    assets->meshArray = up_mesh_list_transferOwnership(&meshArray);
    assets->textureArray = up_texture_list_transferOwnership(&textureArray);
    assets->scaleArray = up_vec3_list_transferOwnership(&scaleArray);

    internal_assets = assets;
    return assets;
}

void up_assets_shutdown_deinit(struct up_assets *assets)
{
    free(assets->meshArray);
    free(assets->textureArray);
    free(assets->scaleArray);
    free(assets);
}
