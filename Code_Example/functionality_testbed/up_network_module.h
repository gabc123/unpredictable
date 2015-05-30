//
//  up_network_module.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_NETWORK_MODULE_H
#define UP_NETWORK_MODULE_H
#include "up_sdl_redirect.h"
#include "up_modelRepresentation.h"
#include "up_ship.h"
#include "up_thread_utilities.h"

#define UP_MAX_CLIENTS 20

struct up_network_datapipe {
    SDL_Thread *recive_thread;
    struct up_thread_queue *queue;
    int online;
    UDPsocket udpSocket;
    //UDPpacket *packet;
    UDPpacket *sendPacket;
    IPaddress addr;
};

struct up_map_data
{
    int playerIndex;
    int mapSeed;
    int numPlayersOnline;
    int playeModel;
    
};

struct up_network_account_data
{
    int playerId;
    unsigned char serverResponse;  //add all data that should be included here
    int noResponse;
    struct up_map_data map;
};

struct up_network_datapipe *up_network_start_gameplay_setup();
struct up_network_datapipe *up_network_start_account_setup();

void up_network_shutdown_deinit(struct up_network_datapipe *p);


int up_network_getNewMovement(struct up_actionState *states,int max,int playerId,struct up_network_datapipe *socket_data);
void up_network_sendNewMovement(struct up_actionState *states, struct up_network_datapipe *socket_data);

int up_network_getAccountData(struct up_network_account_data *data,int max,struct up_network_datapipe *socket_data);

int up_network_registerAccount(char *username, char *password,int length, struct up_network_datapipe *socket_data );
int up_network_loginAccount(char *username, char *password, int length, struct up_network_datapipe *socket_data);

int up_hashText(char *hashed_text,char *text,int length);

#endif /* defined(__testprojectshader__up_network_module__) */
