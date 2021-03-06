//
//  up_star_system.c
//  up_game
//
//  Created by Waleed Hassan on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#include "up_star_system.h"
#include "up_assets.h"
#include "up_utilities.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "up_modelRepresentation.h"
#include <math.h>
#include "up_error.h"
#define UP_RAND_FLOAT_PRECISION 8000
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static void up_generate_sun()
{
    struct up_objectInfo sun = {0};
    sun = up_asset_createObjFromId(3);
    sun.pos.z = 40;
    sun.objectId.type = up_environment_type;
    if(up_unit_add(up_environment_type,sun)==0){
        UP_ERROR_MSG("could not add sun");
    }
}


/*
    gets all object and add 10 new units to be updated to the
 */
static int update_helper(int *progress,enum up_object_type type,struct up_objectID *object_movedArray, int movedObject_count,int max_object_move)
{
    int numObj = 0;
    up_unit_getAllObj(type, &numObj);   // only need the numObj
    
    if (movedObject_count >= max_object_move) {
        return movedObject_count;
    }
    
    int objToAdd = (10 < max_object_move - movedObject_count) ? 10 : 0;
    int i = 0;
    int toIndex = *progress + objToAdd;
    
    if (toIndex >= numObj) {
        toIndex = numObj;
        *progress = 0;
    }
    
    for (i = *progress; i < toIndex; i++) {
        object_movedArray[movedObject_count].idx = i;
        object_movedArray[movedObject_count].type = type;
        
        movedObject_count++;
        (*progress)++;
        if (movedObject_count >= max_object_move) {
            return movedObject_count;
        }
    }
    
    return movedObject_count;
}

/*
    This walks through all object in the star system over many calles, this is to check that all players have the correct map
    it adds the object to be updated to the clients.
 */
int up_server_mapUpdate(struct up_mapUpdateTracker *mapUpdate,struct up_objectID *object_movedArray, int movedObject_count,int max_object_move)
{
    mapUpdate->updateTick++;
    
    if (mapUpdate->updateTick < 200) {
        return movedObject_count;
    }
    mapUpdate->updateTick = 0;
    movedObject_count = update_helper(&mapUpdate->progress_eviorment,up_environment_type,object_movedArray,movedObject_count,max_object_move);
    
    movedObject_count = update_helper(&mapUpdate->progress_projectile,up_projectile_type,object_movedArray,movedObject_count,max_object_move);

    //movedObject_count = update_helper(&mapUpdate->progress_ships,up_ship_type,object_movedArray,movedObject_count,max_object_move);

    return movedObject_count;
}

//waleed
static void up_generate_asteroidBelt(int density,float maxAngle,float minAngle,float outerEdge,float innerEdge,float maxHeight,float minHeight,int seed)
{

    //up_srand((unsigned)time(NULL));
    up_srand(seed);  // same map every time
    struct up_objectInfo asteroid = up_asset_createObjFromId(2);
    asteroid.objectId.type = up_environment_type;
    float angle=0;
    float height=0;
    float radius=0;
    float dx=0;
    float dy=0;
    float scale = 1;

    int i=0;



    for(i=0; i<density; i++){

        angle = minAngle + ((float)(up_rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (maxAngle - minAngle);
        height = minHeight + ((float)(up_rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (maxHeight - minHeight);
        //height = 40;
        radius = innerEdge + ((float)(up_rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (outerEdge - innerEdge);

        dx = cosf(angle);
        dy = sinf(angle);

        asteroid.pos.x = radius * dx;
        asteroid.pos.y = radius * dy;
        asteroid.pos.z = height;
        //scale = 1 + ((float)(up_rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * 1.5f ;
        asteroid.scale.x = scale;
        asteroid.scale.y = scale;
        asteroid.scale.z = scale;
        asteroid.collisionbox.length.x = scale;
        asteroid.collisionbox.length.y = scale;
        asteroid.collisionbox.length.z = scale;

        if(up_unit_add(up_environment_type,asteroid)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }

    }


}

static void up_generate_randomize_box(int density,int seed){
    
    up_srand(seed);
    struct up_objectInfo satellite = up_asset_createObjFromId(7);
    satellite.objectId.idx = 1;
    satellite.objectId.type = up_others_type;
    satellite.spawnTime = 0;
    int i=0;
    for(i=0; i<density; i++){
        
        satellite.pos.x = (float)((up_rand()%1000) - 600);
        satellite.pos.y = (float)((up_rand()%1000) - 600);
        satellite.pos.z = 40;
        //        satellite.scale.x=1;
        //        satellite.scale.y=1;
        //        satellite.scale.z=1;
        
        if(up_unit_add(up_projectile_type,satellite)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }
        
    }
    
    
}

/*
static void up_generate_randomize_satellite(int density,int seed){

    up_srand(seed);
    struct up_objectInfo satellite = up_asset_createObjFromId(7);
    satellite.objectId.idx = 1;
    satellite.objectId.type = up_others_type;
    int i=0;
    for(i=0; i<density; i++){

        satellite.pos.x = (float)((up_rand()%1000) - 500);
        satellite.pos.y = (float)((up_rand()%1000) - 500);

        satellite.scale.x=1;
        satellite.scale.y=1;
        satellite.scale.z=1;

        if(up_unit_add(up_environment_type,satellite)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }

    }




}
*/
static void up_generate_randomize_spaceMine(int density,int seed){

    up_srand(seed);
    struct up_objectInfo mine = up_asset_createObjFromId(8);
    mine.objectId.type = up_environment_type;

    int i=0;
    for(i=0; i<density; i++){

        mine.pos.x = (float)((up_rand()%1000) - 500);
        mine.pos.y = (float)((up_rand()%1000) - 500);
        mine.pos.z = 40;    // so we have a start value

        mine.scale.x=1;
        mine.scale.y=1;
        mine.scale.z=1;

        if(up_unit_add(up_environment_type,mine)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }
    }
}

void up_generate_map(int seed)
{

    up_generate_sun();
    
    up_generate_asteroidBelt(300, 2*M_PI, 0, 500, 440, 50, 30,seed);
    
    up_generate_randomize_box(40,seed);        //satellite
    up_generate_randomize_spaceMine(20,seed);        //space mine


}
