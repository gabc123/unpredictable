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


//both server and client chare these funktion
struct pthread_listen_datapipe{
    UDPsocket udpSocket;
    UDPpacket *packet;
    // char *buffer;
    
};

typedef struct pthread_listen_datapipe Pthread_listen_datapipe;

int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
int up_network_recive(void *information);

UDPsocket get_port_number(int ticker);
int init_sdl();
UDPpacket *memory_check();
void terminate_process(UDPpacket *packet,SDLNet_SocketSet *set);
void error_messages();
SDLNet_SocketSet allo_addsock(Pthread_listen_datapipe *p);
int up_network_recive(void *information);
int check_incoming_data(SDLNet_SocketSet set);

void up_network_start_setup() // used to be main
{
    
    if(up_concurrentQueue_start_setup()==0)
    {
        UP_ERROR_MSG("failed queue startup");
        error_messages();
    }
    
    char file[UP_NETWORK_SIZE]="ip_address";
    char ip_address[UP_NETWORK_SIZE];
    UDPsocket udpSocket;                               //Variabel som hanterar sockets
    UDPpacket *packet;
    SDL_Thread *thread;
    IPaddress addr;
    Pthread_listen_datapipe listen_pipe;
    
    
    int ticker=1;
    
    FILE *fp=fopen(file,"r");
    fscanf(fp,"%s", ip_address);
    //printf("%s", ip_address);
    //initial of sdl_init

    udpSocket=get_port_number(ticker);
    
    if(udpSocket==0){
        printf("could not connect to a port");
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    //resolve socket name
    if(SDLNet_ResolveHost(&addr, ip_address, 5001)==-1){
        printf("Could not fill out ip");
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    //allocate memory malloc
    packet=memory_check();
    if(packet==0){
        printf("could not allocate");
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    listen_pipe.packet =packet;
    listen_pipe.udpSocket =udpSocket;
    
    thread=SDL_CreateThread(up_network_recive,"client_recive_data",&listen_pipe);
    if(thread==NULL){
        printf("create_thread faild: %s", SDL_GetError());
        error_messages();
    }
    //Not sure if this works
    //client_send_data(packet,&udpSocket,addr);
    
    
    //terminate_process(packet,&set);                      //frees the allocated memory
        
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
    Pthread_listen_datapipe *p=(Pthread_listen_datapipe *)information;
    SDLNet_SocketSet set;
    int quit=1,ready;
    printf("start listening\n");
    struct objUpdateInformation obj;
    set=allo_addsock(p);
    char *tmp;
    while (quit!=0){
        
        ready=check_incoming_data(set);
        //printf("Redo: %d", ready);
        
        if(ready){
            
            if(SDLNet_SocketReady(p->udpSocket)){               //when the socket is rdy it returns
                
                if(SDLNet_UDP_Recv(p->udpSocket,p->packet)){
                    // strcpy(p->buffer, (char*)p->packet->data);
                    
                    if(strcmp((char *)p->packet->data, "quit\n")==0){
                        quit=0;                                    //did not recive
                    }
                    
                    printf("Echo: %s\n", (char*)p->packet->data);
                    tmp =(char*)p->packet->data;
                    if (tmp[0] == 'w') {
                        printf("ww");
                        obj.speed.y = 1 ;
                        up_writeToNetworkDatabuffer(&obj);
                    }
                    
                    
                    
                }
            }
        }
        
    }
    
    return 1;
}




int up_network_send(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr)
{
    
    int quit=1;
    char test1[200]="for";
    int counter=0;
    char test2[200]="ever";
    char test3[200]="and never";
    
    
    while(quit!=0){
        
        printf("Command >");
        //fgets(buffer,200,stdin);
        SDL_Delay(1);
        
        packet->address.host=addr.host;
        packet->address.port=addr.port;
        
        if (counter==0) {
            packet->data=(unsigned char*)test1;
            counter++;
        }
        else if(counter==1){
            packet->data=(unsigned char*)test2;
            counter++;
        }
        else if(counter==2){
            packet->data=(unsigned char*)test3;
            counter=0;
        }
               // error_messages();
        
        //packet->data =(unsigned char*) buffer;
        packet->len = (int)strlen((char*)packet->data);
        
        printf("%d", packet->len);
        
        //len=(int)strlen(buffer);
        // if(len>0){
        SDLNet_UDP_Send(*udpSocket,-1, packet);
        // }
        
        if(strcmp("quit\n",(char*)packet->data)==0){
            quit=0;
        }
        SDL_Delay(5);
    }
    
    return 0;
}


///////////////////////////////////////////////////////////



UDPsocket get_port_number(int ticker){
    
    UDPsocket udpSocket;
    
    if(ticker>0){
        udpSocket= SDLNet_UDP_Open(0);                  //OPEN SOCKET
        return udpSocket;
    }
    
    else{
        udpSocket= SDLNet_UDP_Open(5001);
        return udpSocket;
    }
    
}

SDLNet_SocketSet allo_addsock(Pthread_listen_datapipe *p){
    
    int trySocketToSocketSet;
    SDLNet_SocketSet set;
    
    set=SDLNet_AllocSocketSet(16);                      //allocate memory for sockets set
    if(set==0){
        printf("There is no socket set allocated");
        error_messages();
        //exit(EXIT_FAILURE);
    }
    
    trySocketToSocketSet=SDLNet_UDP_AddSocket(set, p->udpSocket);
    if(trySocketToSocketSet==-1){
        printf("Could not add the socket to the socketset");
        error_messages();
        //exit(-2);
    }
    
    return set;
}

int check_incoming_data(SDLNet_SocketSet set){
    int ready;
    
    ready=SDLNet_CheckSockets(set, 100);                    //init of ready
    if(ready==-1){
        error_messages();
        //exit(EXIT_FAILURE);
    }
    return ready;
}


int server_recive_data(void *args){
    
    Pthread_listen_datapipe *p=(Pthread_listen_datapipe *)args;
    SDLNet_SocketSet set;
    int quit=1,ready;
    
    
    set=allo_addsock(p);
    
    while (quit!=0){
        
        ready=check_incoming_data(set);
        
        if(SDLNet_SocketReady(p->udpSocket)){               //when the socket is rdy it returns
            
            if(SDLNet_UDP_Recv(p->udpSocket,p->packet)){
                
                
                if(strcmp((char *)p->packet->data, "quit\n")==0){
                    quit=0;                                    //did not recive
                }
                printf("server: %s\n", (char*)p->packet->data);
            }
            
        }
        
    }
    return 1;                                                   //success
}


void server_send_data(Pthread_listen_datapipe *listen_pipe){
    
    int quit=1;
    
    // Pthread_listen_datapipe p=*listen_pipe;
    
    while(quit!=0){
        
        SDL_Delay(1);
        
        //listen_pipe->packet->len = (int)strlen((char*)listen_pipe->packet->data);
        
        //len = (int)strlen((char*)listen_pipe->buffer);
        //        printf("\nstr len %d\n",len);
        //
        //        strcpy((char*)p.packet->data,p.buffer);
        // printf("%d", len);
        // if(len>0){
        SDLNet_UDP_Send(listen_pipe->udpSocket,-1, listen_pipe->packet);
        // }
        
        if(strcmp("quit\n",(char*)listen_pipe->packet->data)==0){
            quit=0;
        }
        
    }
    
}

int init_sdl(){
    
    //INITIAL OF SDL_NET
    if(SDLNet_Init()<0){
        return 0;
    }
    return 1;
}

UDPpacket *memory_check(){
    
    UDPpacket *packet;                                   //ALLOCATE MEMORY, MALLOC
    
    packet = SDLNet_AllocPacket(1024);
    
    return packet;
}

void terminate_process(UDPpacket *packet,SDLNet_SocketSet *set){
    
    SDLNet_FreeSocketSet(*set);
    SDLNet_FreePacket(packet);
    //SDLNet_Quit();
}

void error_messages(){
    
    time_t current_time=time(0);
    const char *file="error_messages";
    
    FILE *fp=fopen(file,"a");
    fprintf(fp,"Time: %s", ctime(&current_time));
    fprintf(fp,"Error: %s\n", SDL_GetError());
    fprintf(fp,"File: %s\nLine: %d\nMessage: %s\n",__FILE__,__LINE__,__func__);
    fprintf(fp,"\n");
    fclose(fp);
}






















