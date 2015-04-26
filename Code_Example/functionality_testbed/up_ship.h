#include "up_sdl_redirect.h"
#include "up_modelRepresentation.h"
#include "up_shader_module.h"

void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_ship *ship);
void up_updateShipMovment(struct up_ship *ship);
void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();
int UP_eventHandler(struct up_ship *ship,struct shipMovement *movement);
void shipMove(struct shipMovement *movement, struct up_ship *ship);
