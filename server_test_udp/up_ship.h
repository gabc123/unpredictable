#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_type.h"
//#include "up_matrixTransforms.h"
//#include "up_modelRepresentation.h"




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

struct up_actionState
{
    union up_shootingStates fireWeapon;
    union up_movement engine;
    union up_turning maneuver;
    struct up_objectID objectID;
};

#endif
