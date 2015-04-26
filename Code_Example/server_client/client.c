#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2_net/SDL_net.h>
#include "Header.h"
#include <time.h>
#define SIZE 100


int main(){
    
    char file[SIZE]="ip_address";
    char ip_address[SIZE];
    UDPsocket udpSocket;                               //Variabel som hanterar sockets
    UDPpacket *packet;
    SDLNet_SocketSet set;
    SDL_Thread *thread;
    IPaddress addr;
    Pthread_listen_datapipe listen_pipe;
    
    
    int ticker=1;

    FILE *fp=fopen(file,"r");
    fscanf(fp,"%s", ip_address);
    //printf("%s", ip_address);
                                                        //initial of sdl_init
    if(init_sdl()==0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        error_messages();
        exit(EXIT_FAILURE);
    }
                                                        //OPEN random port socket
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
    
    thread=SDL_CreateThread(client_recive_data,"client_recive_data",&listen_pipe);
    if(thread==NULL){
        printf("create_thread faild: %s", SDL_GetError());
        error_messages();
    }
                                                        //Not sure if this works
    client_send_data(packet,&udpSocket,addr);

    
    terminate_process(packet,&set);                      //frees the allocated memory
    

    return 0;
}
