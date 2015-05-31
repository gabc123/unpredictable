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
#include "up_game_communication.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static struct up_shootingFlag *up_game_playerWeapons_setup(int max_players,struct up_eventState *weapons_info)
{
    
    printf("\nweapon loaded\n");
    
    struct up_shootingFlag *player_weapons = malloc(sizeof(struct up_shootingFlag)*max_players);
    if (player_weapons == NULL) {
        UP_ERROR_MSG("failed player_weapons malloc");
        player_weapons = &weapons_info->flags;
    }
    
    int i = 0;
    for (i = 0; i < max_players; i++) {
        player_weapons[i] = weapons_info->flags;
    }
    return player_weapons;
}


static struct up_actionState *up_game_playerActionStates_setup(int max_players)
{
    
    struct up_actionState noState = {0};
    
    struct up_actionState *network_states_data = malloc(sizeof(struct up_actionState)*max_players);
    
    if (network_states_data == NULL) {
        UP_ERROR_MSG("failed network_state malloc");
        network_states_data = &noState;
        return NULL;
    }
    
    int i = 0;
    for (i = 0; i < max_players; i++) {
        network_states_data[i] = noState;
    }
    return network_states_data;
}

static struct up_player_stats *up_game_playerInventory_setup(int max_players)
{
    struct up_player_stats emptyPlayer = {0};
    struct up_player_stats *player_stats = malloc(sizeof(struct up_player_stats)*max_players);
    if (player_stats == NULL) {
        UP_ERROR_MSG("failed player_stats malloc");
        player_stats = &emptyPlayer;
        return NULL;
    }
    
    int i = 0;
    for (i = 0; i < max_players; i++) {
        player_stats[i] = emptyPlayer;
    }
    return player_stats;
    
}

void *up_game_simulation(void *parm)
{
    struct up_game_simulation_com *game_simulation = NULL;
    game_simulation = (struct up_game_simulation_com *)parm;
    
    struct up_interThread_communication *account_interCom = NULL;
    struct up_interThread_communication *gameplay_interCom = NULL;
    
    account_interCom = game_simulation->server_account;
    gameplay_interCom = game_simulation->server_gameplay;
    
    //interCom = (struct up_interThread_communication *)parm;
    
    printf("Game simulation init..\n");
    
    int max_ship_count = 70;
    int max_projectile_count = 200;
    int max_enviroment_count = 500;
    int max_others_count = 200;
    struct up_allCollisions allcollisions = {0};
    
    
    
    up_unit_start_setup(max_ship_count, max_projectile_count, max_enviroment_count, max_others_count);
    
//    int totalNumObjects = max_ship_count + max_projectile_count + max_enviroment_count + max_others_count;

 
    //this will load all the assets (modouls,texturs) specifyed in objIndex
    //be aware that index 0 is always a placeholder for modouls not found and so on
    struct up_assets *assets = up_assets_start_setup();
    printf("past assets\n");
    

    // populate one, to test all relevent subsystems,
    struct up_objectInfo dummyObj = up_asset_createObjFromId(0);
    int dummyIdx = up_unit_add(up_ship_type,dummyObj);
    up_unit_remove(up_ship_type, dummyIdx);
    
    up_generate_map(game_simulation->mapSeed);
    
    
    struct up_eventState weapons_info;
    up_weaponCoolDown_start_setup(&weapons_info);
    printf("\nweapon loaded\n");
    
    struct up_shootingFlag *player_weaponsArray = up_game_playerWeapons_setup(UP_MAX_CLIENTS,&weapons_info);
    struct up_actionState *player_actionArray = up_game_playerActionStates_setup(UP_MAX_CLIENTS);
    struct up_player_stats *player_inventoryArray = up_game_playerInventory_setup(UP_MAX_CLIENTS);

    // if the players action have change then the delta gets updated and sent to the other players
    struct up_actionState *delta_actionArray = up_game_playerActionStates_setup(UP_MAX_CLIENTS);
    
    int map_maxPlayers = UP_MAX_CLIENTS;

    
    // call only to when new player connects
    //up_player_setup(player_stats, currentEvent.flags);
    
    printf("starting main loop\n");

    // non player object updates, (missile collide with astriod)
    int max_object_move = 200;
    struct up_objectID *object_movedArray = malloc(sizeof(struct up_objectID)*max_object_move);
    if (object_movedArray == NULL) {
        UP_ERROR_MSG("failed player_stats malloc");
        max_object_move = 0;
    }

    unsigned int loop_delay = 0;
    int delta_time = 0;
    while(game_simulation->online_signal)
    {
        
        // this inerloop runs until the entire gameloop reches 8 ms,
        // this improves the response time, and also reduces the cpu load from moving all obj
        // at full update rate, 8ms equals about 120 gameloop laps each seconde
        do {
            up_game_communication_getAction(player_actionArray, map_maxPlayers, gameplay_interCom);
            up_server_validate_actions(player_actionArray, player_inventoryArray, player_weaponsArray, map_maxPlayers);
            up_game_communication_sendAction(player_actionArray, delta_actionArray, map_maxPlayers, gameplay_interCom);
            
            usleep(100); // do not need to overhet the cpu =) or flod the buffer
            
            delta_time = up_clock_ms() - loop_delay;
            delta_time = (delta_time > 0) ? delta_time : 0; // the up_clock_ms overflows every 72 min i think
            
        }while (delta_time < 8);
        
        up_updateFrameTickRate();
        loop_delay = up_clock_ms(); // used to fix the inerloop
        
        up_server_update_actions(player_actionArray,player_inventoryArray, map_maxPlayers,&weapons_info);

        up_server_updateMovements();
        up_server_checkCollision(&allcollisions);
        
        up_server_update_playerStats(&allcollisions, player_inventoryArray,&weapons_info.flags,map_maxPlayers);
        
        up_server_handleCollision(&allcollisions,player_inventoryArray,&weapons_info.flags,object_movedArray,max_object_move);
        
        
        // this propaget account joins and levs out to all, and into the internal gamestate
        up_game_communication_getAccount(account_interCom, gameplay_interCom);
        
    }
    printf("Ended main loop\n");

    free(player_weaponsArray);
    free(player_actionArray);
    free(player_inventoryArray);
    free(object_movedArray);
    up_unit_shutdown_deinit();


    up_assets_shutdown_deinit(assets);
    //up_network_shutdown_deinit();
    printf("All up_game_simulation cleanup completed\n");
    return NULL;
}