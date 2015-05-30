//
//  up_server_game_simulation.c
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-26.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_server_game_simulation.h"

#include <stdio.h>
#include <stdlib.h>
#include "up_utilities.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_assets.h"
#include "up_error.h"
#include "up_star_system.h"
#include "up_healthbar.h"
#include "up_server.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void game_simulation()
{
    
    
    printf("Game simulation init..\n");
    
    int max_ship_count = 70;
    int max_projectile_count = 200;
    int max_enviroment_count = 500;
    int max_others_count = 200;
    struct up_allCollisions allcollisions = {0};
    
    
    
    up_unit_start_setup(max_ship_count, max_projectile_count, max_enviroment_count, max_others_count);
    
    int totalNumObjects = max_ship_count + max_projectile_count + max_enviroment_count + max_others_count;
    
    totalNumObjects++;
    totalNumObjects--;
    
    
    struct up_map_data mapData = {0};   // gets filled out from the login sequance in menu
    
    //this will load all the assets (modouls,texturs) specifyed in objIndex
    //be aware that index 0 is always a placeholder for modouls not found and so on
    struct up_assets *assets = up_assets_start_setup();
    printf("past assets\n");
    
    
    
    // this is the start ship, initilazing the startin positions
    struct up_objectInfo tmp_ship = up_asset_createObjFromId(1);
    tmp_ship.pos.x = 440;
    tmp_ship.pos.z = 40;
    tmp_ship.dir.x = 0.03;
    tmp_ship.dir.y = 1.0;
    tmp_ship.angle = 0.0;
    tmp_ship.modelId = 1;
    tmp_ship.objectId.type = up_ship_type;
    tmp_ship.turnSpeed = 1;
    tmp_ship.acceleration = 5;
    //tmp_ship.scale = assets->scaleArray[1];
    
    int shipIndex = 0;
    int shipIndex_tmp = 0;
    int i = 0;
    for (i = 0; i < UP_MAX_CLIENTS; i++) {
        shipIndex_tmp = up_unit_add(up_ship_type,tmp_ship);
    }
    
    
    
    struct up_objectInfo stillObj = {0};
    stillObj.pos.z = 30;
    stillObj.scale = assets->scaleArray[2];
    stillObj.modelId = 2;
    stillObj.objectId.type = up_others_type;
    
    struct up_objectInfo *ship = up_unit_objAtIndex(up_ship_type,shipIndex);
    if (ship == NULL) {
        UP_ERROR_MSG("ship do not exist, use loner ship");
        ship = &tmp_ship;
    }
    ship->pos.x += 10;
    up_unit_add(up_environment_type,stillObj);
    
    up_generate_map(mapData.mapSeed);
    

    struct up_eventState currentEvent = {0};
    struct up_actionState shipAction = {0};
    shipAction.objectID.idx = shipIndex;
    shipAction.objectID.type = up_ship_type;
    
    up_weaponCoolDown_start_setup(&currentEvent);
    printf("out of weapon\n");
    
    int map_maxPlayers = UP_MAX_CLIENTS;
    struct up_actionState noState = {0};
    
    struct up_actionState *network_states_data = malloc(sizeof(struct up_actionState)*UP_MAX_CLIENTS);
    
    if (network_states_data == NULL) {
        UP_ERROR_MSG("failed network_state malloc");
        network_states_data = &noState;
        map_maxPlayers = 0;
    }
    
    
    for (i = 0; i < map_maxPlayers; i++) {
        network_states_data[i] = noState;
    }
    
    struct up_player_stats emptyPlayer = {0};
    struct up_player_stats *player_stats = malloc(sizeof(struct up_player_stats)*UP_MAX_CLIENTS);
    if (player_stats == NULL) {
        UP_ERROR_MSG("failed player_stats malloc");
        player_stats = &emptyPlayer;
        map_maxPlayers = 0;
    }
    
    for (i = 0; i < UP_MAX_CLIENTS; i++) {
        player_stats[i] = emptyPlayer;
    }
    
    // call only to when new player connects
    up_player_setup(player_stats, currentEvent.flags);
    
    printf("starting main loop\n");

    int status = 1;
    //int loop_delay = 0;
    while(status)
    {
        up_updateFrameTickRate();
        //status = UP_eventHandler(&currentEvent,&shipAction,keymap);
      
        up_server_update_actions(network_states_data,player_stats, map_maxPlayers,&currentEvent);
        
        up_updateMovements();
        up_checkCollision(&allcollisions);
        up_handleCollision(&allcollisions,&player_stats,currentEvent.flags);
        
        up_server_update_playerStats(&allcollisions, player_stats,&currentEvent.flags, shipIndex);
        
        
    }
    printf("Ended main loop\n");

    up_unit_shutdown_deinit();


    up_assets_shutdown_deinit(assets);
    //up_network_shutdown_deinit();
    printf("All cleanup completed\n");
}