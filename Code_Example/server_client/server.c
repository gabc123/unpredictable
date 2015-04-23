#include "Header.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define SIZE 1000



int main(int argNum,char* argChar[]){
    
    
    UDPsocket udpSocket;
    UDPpacket *packet;
    SDLNet_SocketSet set;
    Pthread_listen_datapipe listen_pipe;
    SDL_Thread *thread;
    int ticker=0;
    
                                                        //INITIAL OF SDL_NET
    if(init_sdl()==0){
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        error_messages();
    }
                                                        //OPEN SOCKET
    
    udpSocket=get_port_number(ticker);
    if(udpSocket==0){
        
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    

    packet=memory_check();                               //allocate memory 
    if(packet==0){
        printf("could not allocate");
        error_messages();
    }
    
    listen_pipe.packet =packet;
    listen_pipe.set = set;
    listen_pipe.udpSocket =udpSocket;
    
    
    thread=SDL_CreateThread(recive_server_data,"recive_server_data",&listen_pipe);
    if(thread==NULL){
        printf("create_thread faild: %s", SDL_GetError());
        error_messages();
    }
    
    
    server_send_data(&listen_pipe);


    
    //SDL_WaitThread(thread,&thread_return_value);
        
    //recive(packet,udpSocket,&set);
    
    
    terminate_process(packet,&set);
    
    return EXIT_SUCCESS;
    
}