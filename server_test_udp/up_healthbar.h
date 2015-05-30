#ifndef __up_game__up_healthbar__
#define __up_game__up_healthbar__
#include <stdio.h>
#include "up_healthbar.h"
#include "up_ship.h"
#include "up_assets.h"
#include "up_type.h"

struct up_container{
    int current;
    int full;
};

struct up_player_stats
{
    struct up_objectID objectId;
    struct up_container bullets;
    struct up_container missile;
    struct up_container laser;
    struct up_container health;
    struct up_container armor;
};


struct up_shootingFlag;
void up_player_setup(struct up_player_stats *player, struct up_shootingFlag weapons);


#endif  //defined(__up_game__up_healthbar__)
