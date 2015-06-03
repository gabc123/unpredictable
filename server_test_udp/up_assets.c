//
//  up_assets.c
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_assets.h"
#include "up_vertex.h"
#include "up_utilities.h"
#include "up_objectReader.h"
#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"
#include "up_filereader.h"
#include "up_modelRepresentation.h"
#include <string.h>
#include <math.h>



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






/*Returns stored date of the model*/
//Sebastian
struct up_objectInfo up_asset_createObjFromId(int modelId)
{
    struct up_objectInfo obj = {0};
    obj.scale.x = 1;
    obj.scale.y = 1;
    obj.scale.z = 1;
    obj.collisionbox.length.x = 1;
    obj.collisionbox.length.y = 1;
    obj.collisionbox.length.z = 1;
    obj.projectile = 0;

    if(modelId >= internal_assets->numobjects )
    {
        modelId = 0;
    }
    
    obj.modelId = modelId;

    
    //internal_assets is a static global
    obj.scale = internal_assets->scaleArray[modelId];
    obj.collisionbox.length = internal_assets->hitboxLengthArray[modelId];

    //printf("array x%f\ny%f\nz%f\n", obj.collisionbox.length.x,obj.collisionbox.length.y,obj.collisionbox.length.z);
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.x)) ? obj.collisionbox.length.x : obj.maxLength;
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.y)) ? obj.collisionbox.length.y : obj.maxLength;
    obj.maxLength = (fabsf(obj.maxLength) < fabsf(obj.collisionbox.length.z)) ? obj.collisionbox.length.z : obj.maxLength;

     //printf("objmaxlength%f \n", obj.maxLength);


    return obj;
}



/*Reads assets to be loaded from a file*/
//Sebastian
//magnus error handeling
static int loadObjects(struct up_generic_list *scaleArray,
                       struct up_generic_list *hitboxLengthArray)
{
    struct UP_textHandler thafile = up_loadAssetFile("objIndex");
    if (thafile.text == NULL) {
        UP_ERROR_MSG("Failed to load objindex");
        return 0;   // failed to a load objindex
    }

    struct up_modelData item; //stores the information of the object

    char *text = thafile.text;
    char *endLine = "\n";
    char *row;

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


        row = up_token_parser(text, &text, endLine, strlen(endLine));
        if(row == NULL)
        {
            //UP_ERROR_MSG("ERROR, obj String could not be found in loadObjects");
            printf("end of file objIndex\n");
            break;
        }
        sscanf(row,"%f %f %f", &item.hitbox.x, &item.hitbox.y, &item.hitbox.z);

        item.hitbox.x = item.hitbox.x * item.scale.x;
        item.hitbox.y = item.hitbox.y * item.scale.y;
        item.hitbox.z = item.hitbox.z * item.scale.z;

        up_vec3_list_add(hitboxLengthArray, &item.hitbox);
        up_vec3_list_add(scaleArray, &item.scale);

    }while(text <= thafile.text + thafile.length -1);

    up_textHandler_free(&thafile);
    return 1; // success

}

/*loads all stored assets into the gpu*/
//Sebastian
struct up_assets *up_assets_start_setup()
{

    struct up_generic_list *scaleArray= up_vec3_list_new(10);
    struct up_generic_list *hitboxLengthArray= up_vec3_list_new(10);


    struct up_vec3 scale = {1.0, 1.0, 1.0};
    up_vec3_list_add(scaleArray, &scale);
    up_vec3_list_add(hitboxLengthArray, &scale);


    loadObjects(scaleArray,hitboxLengthArray);

    struct up_assets *assets = malloc(sizeof(struct up_assets));
    if (assets == NULL) {
        UP_ERROR_MSG("failure in assets");
        return NULL;
    }
    assets->numobjects = up_vec3_list_count(scaleArray);
    assets->scaleArray = up_vec3_list_transferOwnership(&scaleArray);
    assets->hitboxLengthArray = up_vec3_list_transferOwnership(&hitboxLengthArray);

    internal_assets = assets;
    return assets;
}

void up_assets_shutdown_deinit(struct up_assets *assets)
{
    free(assets->scaleArray);
    free(assets->hitboxLengthArray);
    free(assets);
}
