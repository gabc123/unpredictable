//
//  up_object_actions.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __unpredictable_xcode__up_object_actions__
#define __unpredictable_xcode__up_object_actions__

#include "up_type.h"

//Magnus
enum up_none
{
    none = 0
};

union up_shootingStates
{
    enum up_none none;
    enum
    {
        fireMissile = 1,
        fireBullet,
        fireLaser
    }state;
};


union up_movement
{
    enum up_none none;
    enum
    {
        fwd = 1,
        bwd
    }state;
};

union up_turning
{
    enum up_none none;
    enum
    {
        left = 1,
        right,
        bankLeft,
        bankRight
    }state;
};

//Sebastian
enum collisionType
{
    shipEnviroment,
    projectileShip,
    projectileEnviroment,
    enviromentEnviroment,
    shipShip
};
//Tobias
struct up_actionState
{
    union up_shootingStates fireWeapon;
    union up_movement engine;
    union up_turning maneuver;
    struct up_objectID objectID;
};
#endif /* defined(__unpredictable_xcode__up_object_actions__) */
