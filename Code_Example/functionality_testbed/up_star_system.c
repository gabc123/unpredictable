//
//  up_star_system.c
//  up_game
//
//  Created by Waleed Hassan on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#include "up_star_system.h"
#include "up_assets.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "up_modelRepresentation.h"
#include <math.h>
#include "up_error.h"
#define UP_RAND_FLOAT_PRECISION 8000

void up_generate_map()
{




}

void up_generate_sun()
{
    struct up_objectInfo sun = {0};
    sun.scale.x = 50;
    sun.scale.y = 50;
    sun.scale.z = 50;

    sun.modelId = 3;
    sun.objectId.type = up_environment_type;
    if(up_unit_add(up_environment_type,sun)==0){
        UP_ERROR_MSG("could not add sun");
    }
}


//waleed
void up_generate_asteroidBelt(int density,float maxAngle,float minAngle,float outerEdge,float innerEdge,float maxHeight,float minHeight)
{

    //srand((unsigned)time(NULL));
    srand(42);  // same map every time
    struct up_objectInfo asteroid = up_asset_createObjFromId(2);
    asteroid.objectId.type = up_environment_type;
    float angle=0;
    float height=0;
    float radius=0;
    float dx=0;
    float dy=0;
    float scale = 0;

    int i=0;

    

    for(i=0; i<density; i++){

        angle = minAngle + ((float)(rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (maxAngle - minAngle);
        height = minHeight + ((float)(rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (maxHeight - minHeight);
        //height = 40;
        radius = innerEdge + ((float)(rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * (outerEdge - innerEdge);

        dx = cosf(angle);
        dy = sinf(angle);

        asteroid.pos.x = radius * dx;
        asteroid.pos.y = radius * dy;
        asteroid.pos.z = height;
        //scale = 0.02; //+ ((float)(rand()%UP_RAND_FLOAT_PRECISION) / UP_RAND_FLOAT_PRECISION) * 0.5f ;
        asteroid.scale.x += scale;
        asteroid.scale.y += scale;
        asteroid.scale.z += scale;


        if(up_unit_add(up_environment_type,asteroid)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }

    }


}

void up_generate_randomize_satellite(int density){

    srand((unsigned)time(NULL));
    struct up_objectInfo satellite;
    struct up_objectInfo ship;
    satellite.modelId = 7;
    ship.modelId = 0;
    satellite.objectId.idx = 0;
    satellite.objectId.type = up_others_type;
    ship.objectId.type = up_environment_type;
    int i=0;
    for(i=0; i<density; i++){

        ship.pos.x = satellite.pos.x = (float)((rand()%1000) - 500);
        ship.pos.y =satellite.pos.y = (float)((rand()%1000) - 500);

        ship.scale.x =1;
        ship.scale.y =1;
        ship.scale.z =1;
        satellite.scale.x=1;
        satellite.scale.y=1;
        satellite.scale.z=1;

        if(up_unit_add(up_environment_type,satellite)==0){
            UP_ERROR_MSG("could not add asteroid");
            break;
        }

        if(up_unit_add(up_ship_type,ship)==0){
            UP_ERROR_MSG("could not add ship");
            break;
        }
    }




}

void up_generate_randomize_spaceMine(int density){

    srand((unsigned)time(NULL));
    struct up_objectInfo mine;
    mine.modelId = 8;
    mine.objectId.idx = 0;
    mine.objectId.type = up_environment_type;

    int i=0;
    for(i=0; i<density; i++){

        mine.pos.x = (float)((rand()%1000) - 500);
        mine.pos.y = (float)((rand()%1000) - 500);
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
