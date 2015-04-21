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
    
    get_port_number(udpSocket,ticker);
                                                        //ALLOCATE MEMORY, MALLOC

    if(malloc_memory(packet)==0){
        printf("could no allocate memory");
        exit(EXIT_FAILURE);
    }
    
    recive(packet,&udpSocket);
    
    terminate_process(packet);
    
    return EXIT_SUCCESS;
    
}