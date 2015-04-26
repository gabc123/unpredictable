#include "Header.h"
#include "client_header.h"
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

SDLNet_SocketSet allo_addsock(Pthread_listen_datapipe *p){
    
    int trySocketToSocketSet;
    SDLNet_SocketSet set;

    set=SDLNet_AllocSocketSet(16);                      //allocate memory for sockets set
    if(set==0){
        printf("There is no socket set allocated");
        error_messages();
        exit(EXIT_FAILURE);
    }
        
    trySocketToSocketSet=SDLNet_UDP_AddSocket(set, p->udpSocket);
    if(trySocketToSocketSet==-1){
        printf("Could not add the socket to the socketset");
        error_messages();
        exit(-2);
    }
    
    return set;
}

int check_incoming_data(SDLNet_SocketSet set){
    int ready;
    
    ready=SDLNet_CheckSockets(set, 100);                    //init of ready
    if(ready==-1){
        error_messages();
        exit(EXIT_FAILURE);
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
    SDLNet_Quit();
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






