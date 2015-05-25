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

struct up_network_datapipe {
    SDL_Thread *recive_thread;
    struct up_thread_queue *queue;
    int online;
    UDPsocket udpSocket;
    //UDPpacket *packet;
    UDPpacket *sendPacket;
    IPaddress addr;
};

struct up_network_datapipe *up_network_start_setup();
void up_network_shutdown_deinit();


int up_network_getNewMovement(struct up_actionState *states,int max,int playerId,struct up_network_datapipe *socket_data);
void up_network_sendNewMovement(struct up_actionState *states, struct up_network_datapipe *socket_data);

int up_hashPassword();


#endif /* defined(__testprojectshader__up_network_module__) */
