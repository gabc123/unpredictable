#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"

enum up_none
{
    none = 0
};

enum shootingStates
 {
     fireMissile = 1,
     fireBullet,
     fireLaser
 };

enum movement
 {
     forwrd = 1,
     backwrd
 };

 enum turning
 {
     left = 1,
     right
 };

struct up_actionState
{
    enum shootingStates fireWeapon;
    enum movement engineState;
    enum turning maneuver;
    int objectID;
};

struct cooldownTimer
{
    unsigned int startTime;
    unsigned int coolDown;
};

struct up_shootingFlag
{

    struct cooldownTimer bulletFlag;
    struct cooldownTimer missileFlag;
    struct cooldownTimer laserFlag;
};

void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);
void up_updateShipMovment(struct up_objectInfo *ship);
void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();
int UP_eventHandler(struct up_objectInfo *ship,struct shipMovement *movement);
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif
