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

typedef struct up_health_bar_t{
    
    float greenIndex;
    float redIndex;
    
}up_health_bar_t;


up_health_bar_t healthbar_creation();
void moveHealthBar(int ship_id,up_health_bar_t healing);

#endif  //defined(__up_game__up_healthbar__)
