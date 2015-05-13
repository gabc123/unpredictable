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

struct pthread_listen_datapipe{
    SDL_Thread *recive_thread;
    int online;
    UDPsocket udpSocket;
    //UDPpacket *packet;
    UDPpacket *sendPacket;
    IPaddress addr;
};

typedef struct pthread_listen_datapipe Pthread_listen_datapipe_t;

Pthread_listen_datapipe_t *up_network_start_setup();
void up_network_shutdown_deinit();


int up_network_getNewMovement(struct up_actionState *states,int max);
void up_network_sendNewMovement(struct up_actionState *states, Pthread_listen_datapipe_t *socket_data);




#endif /* defined(__testprojectshader__up_network_module__) */
