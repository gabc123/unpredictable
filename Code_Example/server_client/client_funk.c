#include "client_header.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2_net/SDL_net.h>
#include "Header.h"
#define SIZE 100


int client_recive_data(void *parameters){
    
    Pthread_listen_datapipe *p=(Pthread_listen_datapipe *)parameters;
    SDLNet_SocketSet set;
    int quit=1,ready;
    
    
    set=allo_addsock(p);
    
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
                    
                    
                }
            }
        }
        
    }
    
    return 1;
}





void client_send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr){
    
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
    
}