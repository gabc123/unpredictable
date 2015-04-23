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

int recive_server_data(void *args){
    

    Pthread_listen_datapipe *p=(Pthread_listen_datapipe *)args;
    int quit=1,ready;
    
    (*p).set=SDLNet_AllocSocketSet(16);                      //allocate memory for sockets set
    if((*p).set==0){
        printf("There is no socket set allocated");
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    ready=SDLNet_CheckSockets(p->set, 100);                    //init of ready
    if(ready==-1){
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    while (quit!=0){
    
        if(SDLNet_SocketReady(p->udpSocket)==ready){               //when the socket is rdy it returns
            

            if(SDLNet_UDP_Recv(p->udpSocket,p->packet)){
                
               // p->packet = (char*)p->packet->data;
                
                if(strcmp((char *)p->packet->data, "quit\n")==0){
                    quit=0;                                    //did not recive
                }
            
                printf("Data: %s", p->packet->data);
            }
            SDL_Delay(5);
        }

    }
    return 1;                                                   //success
}

int client_recive_data(void *parameters){
    
    
    Pthread_listen_datapipe *p=(Pthread_listen_datapipe *)parameters;
    int quit=1,ready;
    
    p->set=SDLNet_AllocSocketSet(16);                      //allocate memory for sockets set
    if(p->set==0){
        printf("There is no socket set allocated");
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    ready=SDLNet_CheckSockets(p->set, 100);                    //init of ready
    if(ready==-1){
        error_messages();
        exit(EXIT_FAILURE);
    }
                                        // ifsatsen är fungerar itne!!! retunerar echo forever 
    while (quit!=0){

        if(SDLNet_SocketReady(p->udpSocket)==0){               //when the socket is rdy it returns
            
            SDL_Delay(1);
            
            if(SDLNet_UDP_Recv(p->udpSocket,p->packet)){
                
                if(strcmp((char *)p->packet->data, "quit\n")==0){
                    quit=0;                                    //did not recive
                }
                
                printf("Echo: %s", p->packet->data);
            }

        }

    }

    
    return 1;    
}

void send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr){
    
    int quit=1;
    
    while(quit!=0){
        
        printf("Command >");
        fgets((char*)packet->data,8,stdin);
        
        packet->address.host=addr.host;
        packet->address.port=addr.port;
        
        packet->len = (int)strlen((char*)packet->data);
        SDLNet_UDP_Send(*udpSocket,-1, packet);
        
        if(strcmp("quit\n",(char*)packet->data)==0){
            quit=0;
        }
        SDL_Delay(5);
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



void server_send_data(Pthread_listen_datapipe *listen_pipe){
    
    int quit=1;
    
    while(quit!=0){
        
        SDL_Delay(1);
        listen_pipe->packet->len = (int)strlen((char*)listen_pipe->packet->data);
        SDLNet_UDP_Send(listen_pipe->udpSocket,-1, listen_pipe->packet);
        
        if(strcmp("quit\n",(char*)listen_pipe->packet->data)==0){
            quit=0;
        }

    }
    
}

/*void recive_client_data(Pthread_listen_datapipe p){

(*p).set=SDLNet_AllocSocketSet(16);                      //allocate memory for sockets set
if((*p).set==0){
    printf("There is no socket set allocated");
    error_messages();
    exit(EXIT_FAILURE);
}

ready=SDLNet_CheckSockets(p->set, 100);                    //init of ready

while (quit!=0){
    
    //printf("bajs");
    if(ready==-1){
        error_messages();
        exit(EXIT_FAILURE);
    }
    
    
    if(SDLNet_SocketReady(p->udpSocket)==0){               //when the socket is rdy it returns
        
        
        if(SDLNet_UDP_Recv(p->udpSocket,p->packet)){
            
            // p->packet = (char*)p->packet->data;
            
            if(strcmp((char *)p->packet->data, "quit\n")==0){
                quit=0;                                    //did not recive
            }
            
            printf("Data: %s", p->packet->data);
        }
        SDL_Delay(1);
    }
    
}
return EXIT_SUCCESS;                                                   //success
}*/


