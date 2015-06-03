//
//  up_star_system.h
//  up_game
//
//  Created by Waleed Hassan on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#ifndef __up_game__up_star_system__
#define __up_game__up_star_system__

#include <stdio.h>
#include "up_type.h"

/*
    contains how much of the map has been updated so far
    and the updateTick that has been performed done
 */
struct up_mapUpdateTracker
{
    unsigned int updateTick;
    int progress_projectile;
    int progress_eviorment;
    int progress_ships;
};

/*
 This walks through all object in the star system over many calles, this is to check that all players have the correct map
 it adds the object to be updated to the clients.
 */
int up_server_mapUpdate(struct up_mapUpdateTracker *mapUpdate,struct up_objectID *object_movedArray, int movedObject_count,int max_object_move);

void up_generate_map(int seed);



#endif /* defined(__up_game__up_star_system__) */
