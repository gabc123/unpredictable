#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"

enum shootingStates
 {
     none,
     fireMissile,
     fireBullet,
     fireLaser
 };

 enum movement
 {
     none,
     forwrd,
     backwrd
 };

 enum turning
 {
     none,
     left,
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
void up_updateShipMovment(struct up_objectInfo *ship);
void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();
int UP_eventHandler(struct up_objectInfo *ship,struct shipMovement *movement);
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif
