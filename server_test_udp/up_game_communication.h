//
//  up_network_module.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_NETWORK_MODULE_H
#define UP_NETWORK_MODULE_H
#include "up_modelRepresentation.h"
#include "up_ship.h"
#include "up_thread_utilities.h"
#include "up_server_game_simulation.h"
#include "up_server.h"
#define UP_MAX_CLIENTS 20


struct up_map_data
{
    int playerIndex;
    int mapSeed;
    int numPlayersOnline;
    
};

struct up_network_account_data
{
    int playerId;
    unsigned char serverResponse;  //add all data that should be included here
    int noResponse;
    struct up_map_data map;
};

int up_game_communication_getAction(struct up_actionState *states,int max,struct up_interThread_communication *pipe);

void up_game_communication_sendAction(struct up_actionState *actionArray,struct up_actionState *deltaArray,int numActions,struct up_interThread_communication *pipe);

int up_game_communication_getAccount(struct up_interThread_communication *accountCom,struct up_interThread_communication *gameCom);

int up_hashText(char *hashed_text,char *text,int length);

#endif /* defined(__testprojectshader__up_network_module__) */
