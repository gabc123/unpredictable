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
#include "up_shader_module.h"
#include "up_assets.h"

typedef struct up_health_bar_t{

    float greenIndex;
    float redIndex;

}up_health_bar_t;


up_health_bar_t healthbar_creation();
void moveHealthBar(int ship_id,up_health_bar_t healing);

void up_gamePlayInterface(struct up_font_assets *font_assets,struct shader_module *shader_program);

#endif  //defined(__up_game__up_healthbar__)
