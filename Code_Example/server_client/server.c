#include "Header.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 1000



int main(int argNum,char* argChar[]){
    
    
    UDPsocket udpSocket;
    UDPpacket *packet;
    int ticker=0;
    
                                                        //INITIAL OF SDL_NET
    if(init_sdl()==0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
    }
                                                        //OPEN SOCKET
    
    udpSocket=get_port_number(ticker);
    
    if(udpSocket==0){
        
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    
                                                        //ALLOCATE MEMORY, MALLOC

    packet=memory_check();
        
    if(packet==0){
        printf("could not allocate");
    }
        
    
    recive(packet,udpSocket);
    
    terminate_process(packet);
    
    return EXIT_SUCCESS;
    
}