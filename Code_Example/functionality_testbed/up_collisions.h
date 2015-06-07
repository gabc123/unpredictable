//
//  up_collisions.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __unpredictable_xcode__up_collisions__
#define __unpredictable_xcode__up_collisions__
#include "up_control_events.h"
#include "up_object_handler.h"
#include "up_player.h"

//Sebastian
struct up_collision
{
    int object1;
    int object2;
};
// Structure that contain every hit with every id of the objects
//Sebastian
#define UP_COLLISIONS_MAX 200
struct up_allCollisions
{
    //type of collision
    struct up_collision projectileEnviroment[UP_COLLISIONS_MAX];
    struct up_collision projectileShip[UP_COLLISIONS_MAX];
    struct up_collision shipEnviroment[UP_COLLISIONS_MAX];
    struct up_collision enviromentEnviroment[UP_COLLISIONS_MAX];
    struct up_collision shipShip[UP_COLLISIONS_MAX];
    //current number of objects in each array
    int nrProjectileEnviroment;
    int nrProjectileShip;
    int nrShipEnviroment;
    int nrEnviromentEnviroment;
    int nrShipShip;
};

struct up_player_stats;
void up_handleCollision(struct up_allCollisions *allcollisions,struct up_player_stats *player_stats,struct up_shootingFlag *weapons);

void up_checkCollision(struct up_allCollisions *allcollisions);
#endif /* defined(__unpredictable_xcode__up_collisions__) */
