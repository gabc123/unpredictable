#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"

enum up_none
{
    none = 0
};

union shootingStates
{
    enum up_none none;
    enum
    {
        fireMissile = 1,
        fireBullet,
        fireLaser
    }state;
};

//enum shootingStates
// {
//     fireMissile = 1,
//     fireBullet,
//     fireLaser
// };
union movement
{
    enum up_none none;
    enum
    {
        fwd = 1,
        bwd
    }state;
};

union turning
{
    enum up_none none;
    enum
    {
        left = 1,
        right
    }state;
};

struct up_objectID
{
    int idx;
    enum up_object_type type;
};

struct up_actionState
{
    union shootingStates fireWeapon;
    union movement engine;
    union turning maneuver;
    struct up_objectID objectID;
};

struct cooldownTimer
{
    unsigned int startTime;
    unsigned int coolDown;
    unsigned int amunitionSpeed;
};

struct up_shootingFlag
{

    struct cooldownTimer bulletFlag;
    struct cooldownTimer missileFlag;
    struct cooldownTimer laserFlag;
};

struct up_eventState
{
    struct up_shootingFlag flags;
};

void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);

void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server, int nrObj, struct up_eventState *funkarEj);

//this funktion updates the global position of all objects in the world
// only called in the main gameloop once
void up_updateMovements();

void up_updateShipMovment(struct up_objectInfo *ship);

void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();

void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent);
int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction);

void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif
