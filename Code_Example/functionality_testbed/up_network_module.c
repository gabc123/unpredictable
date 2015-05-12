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
#include <string.h>
#include "up_thread_utilities.h"
#include "up_error.h"

#define UP_NETWORK_SIZE 100


int up_network_recive(void *information);

// warning:
// this function copy the data from source into destination,
// this function is pure evil and copy raw byte data of element_size,
// thid function do not check if destination and source is pointing to valid data
// and it do not do any bound checking
static void generic_copyElement(unsigned int element_size,unsigned char *destination,unsigned char *source)
{
    unsigned char *end = destination + element_size;
    for (; destination < end; destination++,source++) {
        *destination = *source;
    }
}

unsigned int up_copyObjectIntoBuffer(struct objUpdateInformation *object,unsigned char *buffer)
{
    unsigned int data_len = sizeof(object->data);
    generic_copyElement(data_len,buffer,(unsigned char *)&object->data);
    return data_len;
}

unsigned int  up_copyBufferIntoObject(unsigned char *buffer,struct objUpdateInformation *object)
{
    unsigned int data_len = sizeof(object->data);
    generic_copyElement(data_len,(unsigned char *)&object->data,buffer);
    return data_len;
}


Pthread_listen_datapipe_t *up_network_start_setup()                               // used to be main
{
    Pthread_listen_datapipe_t *p = malloc(sizeof(Pthread_listen_datapipe_t));
    if (p == NULL) {
        UP_ERROR_MSG("network setup failed malloc");
        return NULL;
    }
    UDPsocket client_socket;                                // deklaratons of sdl udp sockets for later use
    //UDPpacket *recive_packet;
    //IPaddress addr;
    SDL_Thread *thread;
    char file[20]="ip_address";
    char ip_address[20];
    int success;
    
                                                            //need to be started early to make sure that it is up and running before the main game loop begins
    if(up_concurrentQueue_start_setup()==0)                 //recived queu buffert so that important coordinates does not rewrites
    {
        UP_ERROR_MSG("failed queue startup");
    }
    else{
        printf("Success queue");
    }
    
    FILE *fp=fopen(file,"r");                               // open file, read the content, scans the file and push it into the array
    if (fp == NULL) {
        UP_ERROR_MSG_STR("Missing file, ip_address,network failure",file);
        strcpy(ip_address, "127.0.0.1");                    // this make sure the we have a fallback address, the loopback
    }else
    {
        fscanf(fp,"%s", ip_address);
        fclose(fp);
    }
    printf("%s", ip_address);

                                                            // initialize of sdlnet_init, returns -1 if not success else success
    
    success=SDLNet_Init();
    if(success==-1){
        printf("Error: Initialize not success");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Initialize success");
    }
                                                            // randomize a port so that the client can communicate, returns 0 if not success else success
    
    client_socket=SDLNet_UDP_Open(0);
    if(!client_socket){
        printf("Error: Could not open socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Socket port: ");
    }
                                                            // resolving conection between host and destination address, sending data throuh port 5001
    
    success=SDLNet_ResolveHost(&p->addr, ip_address, 5001);
    if(success==-1){
        printf("Error: Could not fill out ip address");
    }
    else{
        printf("Success filling out the ip address");
    }
    
    
                                                            // allocate and resize a single udp packet
    
                                                            // creates thread for the server
    thread=SDL_CreateThread(up_network_recive,"up_network_recive",p);
    
    
    
    return p;
}



void up_network_shutdown_deinit()
{
    up_concurrentQueue_shutdown_deinit();
    
}


void up_newtwork_getNewMovement(struct up_objectInfo * ship)
{
    struct objUpdateInformation obj[5];
    
    int len = up_readNetworkDatabuffer(obj, 5);
    int i = 0;
    for (i=0; i<len; i++) {
        //ship->speed += obj[i].speed.y;
    }
}


int up_network_recive(void *arg)
{
    Pthread_listen_datapipe_t *p=(Pthread_listen_datapipe_t *)arg;
    int quit=1;
    UDPsocket socket = p->udpSocket;
    UDPpacket *packet = SDLNet_AllocPacket(1024);
    struct objUpdateInformation local_data = {0};
    
    while(quit!=0){
        SDL_Delay(1);

        if (SDLNet_UDP_Recv(socket,packet)){
            
            printf("UDP Packet incoming\n");
            printf("\tChan:    %d\n", packet->channel);
            //printf("\tData:    %s\n", (char *)packet->data);
            printf("\tLen:     %d\n", packet->len);
            printf("\tMaxlen:  %d\n", packet->maxlen);
            printf("\tStatus:  %d\n", packet->status);
            printf("\tAddress: %x %x\n", packet->address.host, packet->address.port);
            
            if (packet->len >= sizeof(local_data.data)) {
                up_copyBufferIntoObject(packet->data,&local_data);
                
                up_writeToNetworkDatabuffer(&local_data);
            }
        
        }
        
        
        
        SDL_Delay(1);

    }
    

    return 1;
}




int up_network_send(UDPsocket *udpSocket,IPaddress addr)
{

    return 0;

}
