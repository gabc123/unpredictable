#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2_net/SDL_net.h>
#include "Header.h"
#define SIZE 100



int main(){
    
    char file[SIZE]="ip_address";
    char ip_address[SIZE];
    UDPsocket udpSocket;                               //Variabel som hanterar sockets
    UDPpacket *packet;
    IPaddress addr;
    int ticker=1;
    
    FILE *fp=fopen(file,"r");
    fscanf(fp,"%s",ip_address);
    printf("%s", ip_address);
    fclose(fp);
                                                        //initial of sdl_init
    if(init_sdl()==0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
    }
                                                        //OPEN random port socket
    get_port_number(&udpSocket,ticker);
    printf("\nmain recv loop started");
                                                       //resolve socket name
    if(SDLNet_ResolveHost(&addr, ip_address, 5001)==-1){
        printf("Could not fill out ip");
        exit(EXIT_FAILURE);
    }
    
                                                        //allocate memory malloc
    if(malloc_memory(&packet)==0){
        fprintf(stderr,"SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    
        
    send_data(packet,&udpSocket,addr);
    
    
    terminate_process(packet);

    return 0;
}
