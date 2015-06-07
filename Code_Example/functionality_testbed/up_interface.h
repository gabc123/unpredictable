#ifndef __up_game__up_healthbar__
#define __up_game__up_healthbar__

#include "up_math.h"
#include "up_assets.h"
#include "up_object_handler.h"
#include "up_player.h"
#define UP_NUMBER_OF_SYMBOLS 10

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

struct up_interface_radar {
    int modelId;
    struct up_vec3 pos;
    struct up_vec3 scale;
    float enemyAngle;
};


struct up_interface_game{
    
    struct up_interface_stats playerStats;
    struct up_interface_symbols symbolArray[UP_NUMBER_OF_SYMBOLS];
    struct up_interface_bar health;
    struct up_interface_bar armor;
    struct up_interface_radar radar;
    int countSymbol;
    
};

// returns the angle between ship and the closes ship
float up_radar_search(struct up_interface_game *interface,struct up_assets *assets,struct up_objectInfo *ship);

void up_interface_gamePlay(struct up_assets *assets ,struct up_font_assets *font_assets,
                           struct up_shader_module *shader_program,struct up_interface_game *interface);

void up_interface_creation(struct up_interface_game *interface, struct up_player_stats *player);


void up_interface_update(struct up_interface_game *inteface, struct up_player_stats *player);

#endif  //defined(__up_game__up_healthbar__)
