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

/*stores information of the object*/
//Sebastian
struct up_modelData
{
    char obj[MODELLENGTH];
    char tex[MODELLENGTH];
    struct up_vec3 scale;
    struct up_vec3 hitbox;
    float turnSpeed;
    float maxLength;
};

/*fallback mesh*/
//magnus
struct up_mesh *dummyobj()
{
    struct up_mesh *mesh = meshTriangleShip();
    return mesh;
}

/*Returns stored date of the model*/
//Sebastian
struct up_objectInfo up_asset_createObjFromId(int modelId)
{
    //set fallback values
    struct up_objectInfo obj = {0};
    obj.scale.x = 1;
    obj.scale.y = 1;
    obj.scale.z = 1;
    obj.collisionbox.length.x = 1;
    obj.collisionbox.length.y = 1;
    obj.collisionbox.length.z = 1;
    obj.projectile = 0;

    //fallback mesh is to be used if assets failed to load the object from the objIndex file
    if(modelId >= internal_assets->numobjects )
    {
        modelId = 0;
    }

    obj.modelId = modelId;

    //internal_assets is a static global and proud of it :-)
    obj.scale = internal_assets->scaleArray[modelId];
    obj.collisionbox.length = internal_assets->hitboxLengthArray[modelId];

    //printf("array x%f\ny%f\nz%f\n", obj.collisionbox.length.x,obj.collisionbox.length.y,obj.collisionbox.length.z);
    //maxlength is the value to be used for the collisionsphere
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.x)) ? obj.collisionbox.length.x : obj.maxLength;
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.y)) ? obj.collisionbox.length.y : obj.maxLength;
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.z)) ? obj.collisionbox.length.z : obj.maxLength;

     //printf("objmaxlength%f \n", obj.maxLength);

    return obj;
}

//magnus
int up_process_asset(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_modelData *item)
{
    int returnCode = 1;
    struct up_objModel *testObj = NULL;
    struct up_mesh *mesh = NULL;
    struct up_texture_data *texture = NULL;

    // set up the dummyobjects to be used in case of failure
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

/*Reads assets to be loaded from a file*/
//Sebastian
//magnus error handeling
static int loadObjects(struct up_generic_list *meshArray,
                       struct up_generic_list *textureArray,
                       struct up_generic_list *scaleArray,
                       struct up_generic_list *hitboxLengthArray)
{
    //objIndex stores information that is to be loaded into the gpu
    struct UP_textHandler thafile = up_loadAssetFile("objIndex");
    if (thafile.text == NULL) {
        UP_ERROR_MSG("Failed to load objindex");
        return 0;   // failed to a load objindex
    }

    struct up_vec3 scaleOne = {1.0, 1.0, 1.0};
    struct up_modelData item; //stores the information of the object

    char *text = thafile.text;
    char *endLine = "\n";
    char *row;

    /*reads from the file and stores read data*/
    //token_parser reads one line of text and puts the pointer unto the next line until end of file
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

        row = up_token_parser(text, &text, endLine, strlen(endLine));
        if(row == NULL)
        {
            //UP_ERROR_MSG("ERROR, obj String could not be found in loadObjects");
            printf("end of file objIndex\n");
            break;
        }
        sscanf(row,"%f %f %f", &item.hitbox.x, &item.hitbox.y, &item.hitbox.z);

        //hitbox scales with modelscale
        item.hitbox.x = item.hitbox.x * item.scale.x;
        item.hitbox.y = item.hitbox.y * item.scale.y;
        item.hitbox.z = item.hitbox.z * item.scale.z;

        up_vec3_list_add(hitboxLengthArray, &item.hitbox);
        up_vec3_list_add(scaleArray, &item.scale);

    }while(text <= thafile.text + thafile.length -1);

    up_textHandler_free(&thafile);
    return 1; // success

}

/*Returns all stored assets into the gpu*/
//Sebastian
struct up_assets *up_assets_start_setup()
{
    struct up_generic_list *meshArray = up_mesh_list_new(10);
    struct up_generic_list *textureArray = up_texture_list_new(10);
    struct up_generic_list *scaleArray= up_vec3_list_new(10);
    struct up_generic_list *hitboxLengthArray= up_vec3_list_new(10);

    // the first model is always a default model that is used if
    // a missing obj file or texture is found
    struct up_mesh *mesh = dummyobj();
    struct up_texture_data *texture = up_load_texture("lala.png");
    if ((mesh == NULL) || (texture == NULL)) {
        UP_ERROR_MSG("Major problem , default texture or mesh failed to load, expect segfaults incoming");
    }
    struct up_vec3 scale = {1.0, 1.0, 1.0};

    //Allocates dynamic memory that can expand when nessecary
    up_mesh_list_add(meshArray, mesh);
    up_texture_list_add(textureArray, texture);
    up_vec3_list_add(scaleArray, &scale);
    up_vec3_list_add(hitboxLengthArray, &scale);

    //Loads the arrays with content
    loadObjects(meshArray, textureArray, scaleArray, hitboxLengthArray);

    struct up_assets *assets = malloc(sizeof(struct up_assets));
    if (assets == NULL) {
        UP_ERROR_MSG("failure in assets");
        return NULL;
    }
    assets->numobjects = up_mesh_list_count(meshArray);
    assets->meshArray = up_mesh_list_transferOwnership(&meshArray);
    assets->textureArray = up_texture_list_transferOwnership(&textureArray);
    assets->scaleArray = up_vec3_list_transferOwnership(&scaleArray);
    assets->hitboxLengthArray = up_vec3_list_transferOwnership(&hitboxLengthArray);

    //static global too keep track of the assets internally
    internal_assets = assets;

    //contain all information of the loaded models
    return assets;
}

//Magnus
void up_assets_shutdown_deinit(struct up_assets *assets)
{
    free(assets->meshArray);
    free(assets->textureArray);
    free(assets->scaleArray);
    free(assets->hitboxLengthArray);
    free(assets);
}
