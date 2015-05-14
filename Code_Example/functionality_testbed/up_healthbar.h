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

typedef struct up_health_bar{

    int greenIndex;
    int redIndex;
    int heartIndex;

}up_health_bar_t;

typedef struct up_stats_index{
    
    int armorIndex;
    int heartIndex;
    
}up_stats_index_t;

typedef struct up_player_stats{
    
    int current_health;
    int max_health;
    int current_armor;
    int max_armor;

}up_player_stats_t;


up_health_bar_t healthbar_creation();                           //creates healthbar
void up_moveHealthBar(int ship_id,up_health_bar_t healing);      //puts the right position for it

up_stats_index_t up_create_statsObject();
void up_interface_placement(struct up_camera *cam,up_stats_index_t interfaceObject);

void up_gamePlayInterface(struct up_font_assets *font_assets,struct shader_module *shader_program,up_player_stats_t stats);


#endif  //defined(__up_game__up_healthbar__)
