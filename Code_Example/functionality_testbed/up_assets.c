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


void up_process_asset(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_modelData *item)
{
    struct up_objModel *testObj = NULL;
    struct up_mesh *mesh = NULL;
    struct up_texture_data *texture = NULL;

    //struct load item;
    testObj= up_loadObjModel(item->obj);

    mesh = UP_mesh_new(testObj->vertex, testObj->vertex_length, testObj->indexArray, testObj->index_length);
    if (mesh == NULL) {
        mesh = dummyobj();
    }
    up_mesh_list_add(meshArray, mesh);

    up_objModelFree(testObj);
    texture = up_load_texture(item->tex);

    if (texture == NULL) {
        texture=up_load_texture("lala.png");
    }

    up_texture_list_add(textureArray, texture);
}

struct up_assets *up_assets_start_setup()
{

    struct up_generic_list *meshArray = up_mesh_list_new(10);
    struct up_generic_list *textureArray = up_texture_list_new(10);
    struct up_generic_list *scaleArray= up_vec3_list_new(10);

    loadObjects(meshArray, textureArray, scaleArray);

    struct up_assets *assets = malloc(sizeof(struct up_assets));
    assets->meshArray = up_mesh_list_transferOwnership(&meshArray);
    assets->textureArray = up_texture_list_transferOwnership(&textureArray);
    assets->scale=up_vec3_list_transferOwnership(&scaleArray);

    return assets;
}

/*
    loadobject loads models listed in the objIndex file.
*/

void loadObjects(struct up_generic_list *meshArray, struct up_generic_list *textureArray, struct up_generic_list *scaleArray)
{

    struct UP_textHandler thafile = up_loadAssetFile("objIndex");

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
        sscanf(rad,"%f %f %f %s %s", item.scale.x, item.scale.y, item.scale.z, item.obj, item.tex);
        up_process_asset(meshArray,textureArray,&item);

      /*scale.x=item.scale.x;
        scale.y=item.scale.y;
        scale.z=item.scale.z;*/

    }while(text <= thafile.text + thafile.length -1);
    up_textHandler_free(&thafile);
    up_vec3_list_add(scaleArray, &item.scale);

}
