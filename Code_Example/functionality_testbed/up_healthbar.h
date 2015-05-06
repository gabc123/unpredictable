#ifndef __up_game__up_healthbar__
#define __up_game__up_healthbar__
#include <stdio.h>
#include "up_healthbar.h"
#include "up_matrixTransforms.h"
#include "up_ship.h"
#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_ship.h"
#include "up_texture_module.h"




int healthbar_creation();
void moveHealthBar(struct up_objectInfo *ship,int index);

#endif  //defined(__up_game__up_healthbar__)
