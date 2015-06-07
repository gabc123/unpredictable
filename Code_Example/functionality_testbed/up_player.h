//
//  up_player.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __unpredictable_xcode__up_player__
#define __unpredictable_xcode__up_player__
#include "up_control_events.h"
#include "up_object_handler.h"

#include "up_music.h"
struct up_container{
    int current;
    int full;
};

struct up_player_stats
{
    
    struct up_container bullets;
    struct up_container missile;
    struct up_container laser;
    struct up_container health;
    struct up_container armor;
};

void up_updateMovements();

/*determine the new direction and speed of the object*/
//turnspeed is a set value atm. It is to be stored for each obj
//Sebastian 2015-05-05
//Magnus 2015-05-06
//walid
//Sebastian
void up_moveObj(struct up_objectInfo *localObject, struct up_actionState *obj, double frameDelta);
/*Creates the fired projectiles adding the speed and direction of the ship that fired them*/
//Sebastian 2015-05-05
void up_createProjectile(struct up_objectInfo *localobject,
                         struct up_actionState *obj, struct up_eventState *ammoStats,
                         struct soundLib *sound);
/*updates all action changes in the game*/
//Sebastian 2015-05-05
void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server,
                       int nrObj, struct up_eventState *ammoStats, struct soundLib *sound);

void up_player_setup(struct up_player_stats *player, struct up_shootingFlag weapons);

struct up_allCollisions;
//walled
// magnus included weapons so everything gets synced to the same state
void up_update_playerStats(struct up_allCollisions *collision,struct up_player_stats *player,struct up_shootingFlag *weapons, int playerId);

#endif /* defined(__unpredictable_xcode__up_player__) */
