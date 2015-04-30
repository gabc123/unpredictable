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

struct pthread_listen_datapipe{
    UDPsocket udpSocket;
    UDPpacket *packet;
    IPaddress addr;
};
typedef struct pthread_listen_datapipe Pthread_listen_datapipe_t;


int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
int up_network_recive(void *information);


void up_network_start_setup()                               // used to be main
{
    Pthread_listen_datapipe_t p;
    UDPsocket client_socket;                                // deklaratons of sdl udp sockets for later use
    UDPpacket *recive_packet;
    IPaddress addr = p.addr;
    SDL_Thread *thread;
    char file[20]="ip_address";
    char ip_address[20];
    int success;
    //need to be started early to make sure that it is up and running before the main game loop begins
    if(up_concurrentQueue_start_setup()==0)                 //recived queu buffert so that important coordinates does not rewrites
    {
        UP_ERROR_MSG("failed queue startup");
        //error_messages();
    }
    
    FILE *fp=fopen(file,"r");                               // open file, read the content, scans the file and push it into the array
    if (fp == NULL) {
        UP_ERROR_MSG_STR("Missing file, ip_address,network failure",file);
        strcpy(ip_address, "127.0.0.1");    // this make sure the we have a fallback address, the loopback
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
    
    client_socket=SDLNet_UDP_Open(5001);
    if(!client_socket){
        printf("Error: Could not open socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Socket port: ");
    }
                                                            // resolving conection between host and destination address, sending data throuh port 5001
    
//    success=SDLNet_ResolveHost(&addr, ip_address, 5001);
//    if(success==-1){
//        printf("Error: Could not fill out ip address");
//    }
//    else{
//        printf("Success filling out the ip address");
//    }
    
    
                                                            // allocate and resize a single udp packet
    recive_packet=SDLNet_AllocPacket(1024);
    if(recive_packet==NULL){
        printf("Error: Ran out of memory");
    }
    else{
        printf("Allocation success");
    }
    
    p.packet =recive_packet;
    p.udpSocket =client_socket;                                                       // puts the host and port in the stucture
    printf("recvp : %p\n", p.packet);
    p.packet->address.host=addr.host;
    p.packet->address.port=addr.port;
    
                                                            // creates thread for the server
    thread=SDL_CreateThread(up_network_recive,"up_network_recive",&p);
    
    
    
    
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


int up_network_recive(void *arg)
{
    Pthread_listen_datapipe_t *p=(Pthread_listen_datapipe_t *)arg;
    struct objUpdateInformation obj;
    char tmp[20];
    char zero[20]={0};
    int quit=1;
    UDPsocket socket =p->udpSocket;
    UDPpacket *packet =p->packet;
    //unsigned char kuk[20]="mamma knullare";
    
    while(quit!=0){
    SDL_Delay(10);
        

        
        packet->address.host=p->addr.host;
        packet->address.port=p->addr.port;
        
        if (SDLNet_UDP_Recv(socket,packet)){
            
            printf("UDP Packet incoming\n");
            printf("\tChan:    %d\n", packet->channel);
            printf("\tData:    %s\n", (char *)packet->data);
            printf("\tLen:     %d\n", packet->len);
            printf("\tMaxlen:  %d\n", packet->maxlen);
            printf("\tStatus:  %d\n", packet->status);
            printf("\tAddress: %x %x\n", packet->address.host, packet->address.port);
            
            
            //fgets(tmp,20,stdin);
            
            if(strcmp((char *)packet->data,"ww\n")==0){
                printf("ww");
                obj.speed.y = 20 ;
                up_writeToNetworkDatabuffer(&obj);
            }
            else if(strcmp(tmp,"quit\n")==0){
                quit=0;
            }
        
        
        }
        
        
        
        
        SDL_Delay(1);
       // fgets((char*)kuk,20,stdin);

        
        //packet->data=kuk;
        
        packet->len = (int)strlen((char*)packet->data);
        
        SDLNet_UDP_Send(socket,-1, packet);
        // }
        
//        if(strcmp("quit\n",(char*)p->packet->data)==0){
//            quit=0;
//        }
        
        //memcpy(packet->data,zero,20);
    }
    

    return 1;
}




//int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr)
//{
//    
//    
//    return 0;
//}
//}
