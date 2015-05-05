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
     fwd = 1,
     bwd
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


void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);

//this funktion updates the global position of all objects in the world
// only called in the main gameloop once
void up_updateMovements();

void up_updateShipMovment(struct up_objectInfo *ship);

void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();
int UP_eventHandler(struct up_objectInfo *ship,struct shipMovement *movement);
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif
