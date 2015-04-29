//
//  up_network_module.c
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_network_module.h"
#include "up_sdl_redirect.h"
#include "up_modelRepresentation.h"
#include <time.h>
#include <stdlib.h>
#include "up_thread_utilities.h"
#include "up_error.h"

#define UP_NETWORK_SIZE 100



int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
int up_network_recive(void *information);


void up_network_start_setup() // used to be main
{
    
    if(up_concurrentQueue_start_setup()==0)
    {
        UP_ERROR_MSG("failed queue startup");
        //error_messages();
    }
    
}



void up_network_shutdown_deinit()
{
    up_concurrentQueue_shutdown_deinit();
    
}


void up_newtwork_getNewMovement(struct up_ship * ship)
{
    struct objUpdateInformation obj[5];
    
    int len = up_readNetworkDatabuffer(obj, 5);
    int i = 0;
    for (i=0; i<len; i++) {
        ship->speed += obj[i].speed.y;
    }
}


int up_network_recive(void *information)
{
    struct objUpdateInformation obj;
    
    // if
    printf("ww");
    obj.speed.y = 1 ;
    
    up_writeToNetworkDatabuffer(&obj);
    
    return 1;
}




int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr)
{
    
    
    return 0;
}

