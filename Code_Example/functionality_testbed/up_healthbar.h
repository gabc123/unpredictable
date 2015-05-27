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

//struct up_health_bar{
//
//    int greenModelId;
//    int redModelId;
//    int scale;
//
//};

struct up_interface_symbols{
    
    int modelId;
    struct up_vec3 pos;
    struct up_vec3 scale;
    
};

struct up_interface_inventory{
    
    struct up_vec3 pos;
    struct up_vec3 scale;
    int min;
    int max;
};

struct up_player_stats{
    
    struct up_interface_inventory missile;
    struct up_interface_inventory bullets;
    struct up_interface_inventory lazer;
    struct up_interface_inventory health;
    struct up_interface_inventory ammunitions;
    
};

//up_health_bar_t healthbar_creation();                           //creates healthbar
//void up_moveHealthBar(int ship_id,up_health_bar_t green_and_red, struct up_player_stats *player_stats,struct up_camera *cam); //puts the right position for it
//
//up_stats_index_t up_create_statsObject();
//void up_interface_placement(struct up_camera *cam,up_stats_index_t interfaceObject);

void up_gamePlayInterface(struct up_assets *assets ,struct up_font_assets *font_assets,struct shader_module *shader_program,
                          struct up_player_stats *stats);


#endif  //defined(__up_game__up_healthbar__)
