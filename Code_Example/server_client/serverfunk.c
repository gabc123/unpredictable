#include "Header.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 100



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


void recive(UDPpacket *packet,UDPsocket udpSocket){
    
    int quit=1;
    
    while (quit!=0){
        
        if(SDLNet_UDP_Recv(udpSocket,packet)){
        
            printf("UDP packet is coming\n");
            printf("Chanel: %d", packet->channel);
            printf("Data: %s\n", (char *)packet->data);
            printf("Len: %d\n", packet->len);
            printf("\tMaxlen:  %d\n", packet->maxlen);
            printf("\tStatus:  %d\n", packet->status);
            printf("\tAddress: %x %x\n", packet->address.host, packet->address.port);
        
            if(strcmp((char *)packet->data, "quit")==0){
                quit=0;                                    //did not recive
            }
        }
    }
                                                            //success
}

void send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr){
    
    int quit=1;
    
    while(quit!=0){
        
        printf("Command >");
        fgets((char*)packet->data,8,stdin);
        
        packet->address.host=addr.host;
        packet->address.port=addr.port;
        
        packet->len = strlen(packet->data) + 1;
        SDLNet_UDP_Send(*udpSocket,packet->channel, packet);
        
        if(strcmp("quit",(char*)packet->data)==0){
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

void terminate_process(UDPpacket *packet){
    
    SDLNet_FreePacket(packet);
    SDLNet_Quit();
}
