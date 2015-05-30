#include "up_healthbar.h"
#include "up_ship.h"
#include <stdio.h>
#include "up_utilities.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_modelRepresentation.h"
#include "up_assets.h"
#include <math.h>
#include <stdlib.h>
#include "up_error.h"
 

struct up_vec3 up_set_vec3(float x, float y, float z){
    
    struct up_vec3 vec = {x,y,z};
    
    return vec;
}


void up_player_setup(struct up_player_stats *player, struct up_shootingFlag weapons){
    player->health.current = 100;
    player->health.full = 100;
    player->armor.current = 100;
    player->armor.full = 100;
    
    player->bullets.full = weapons.bulletFlag.ammunition;
    player->bullets.current = weapons.bulletFlag.ammunition;
    player->missile.full = weapons.missileFlag.ammunition;
    player->missile.current = weapons.missileFlag.ammunition;
    player->laser.full = weapons.laserFlag.ammunition;
    player->laser.current = weapons.laserFlag.ammunition;
    
}








