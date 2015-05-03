#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"

void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);
void up_updateShipMovment(struct up_objectInfo *ship);
void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();
int UP_eventHandler(struct up_objectInfo *ship,struct shipMovement *movement);
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif