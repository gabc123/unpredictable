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

struct up_interface_bar{

    int fullModelId;
    int emptyModelId;
    struct up_vec3 pos;
    struct up_vec3 scale;
    float level;

};

struct up_interface_symbols{
    
    int modelId;
    struct up_vec3 pos;
    struct up_vec3 scale;
    
};

struct up_interface_inventory{
    
    struct up_vec3 pos;
    struct up_vec3 scale;
    struct up_vec3 color;
    int current;
    int full;
};

struct up_interface_stats{
    
    struct up_interface_inventory missile;
    struct up_interface_inventory bullets;
    struct up_interface_inventory laser;
    struct up_interface_inventory health;
    struct up_interface_inventory armor;
    
};

struct up_interface_game{
    
    struct up_interface_stats playerStats;
    struct up_interface_symbols symbolArray[10];
    struct up_interface_bar health;
    struct up_interface_bar armor;
    int countSymbol;
    
};

//up_health_bar_t healthbar_creation();                           //creates healthbar
//void up_moveHealthBar(int ship_id,up_health_bar_t green_and_red, struct up_player_stats *player_stats,struct up_camera *cam); //puts the right position for it
//
//up_stats_index_t up_create_statsObject();
//void up_interface_placement(struct up_camera *cam,up_stats_index_t interfaceObject);

struct up_container{
    int current;
    int full;
};
struct up_player_stats{
    
    struct up_container bullets;
    struct up_container missile;
    struct up_container laser;
    struct up_container health;
    struct up_container armor;
};

void up_interface_gamePlay(struct up_assets *assets ,struct up_font_assets *font_assets,struct shader_module *shader_program,
                          struct up_interface_game *interface);

void up_interface_creation(struct up_interface_game *interface, struct up_player_stats *player);
void up_player_setup(struct up_player_stats *player, struct up_shootingFlag weapons);
void up_interface_update(struct up_interface_game *inteface, struct up_player_stats *player);

#endif  //defined(__up_game__up_healthbar__)
