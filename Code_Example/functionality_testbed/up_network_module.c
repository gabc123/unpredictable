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
#include <string.h>
#include "up_thread_utilities.h"
#include "up_error.h"
#include "up_network_packet_utilities.h"

#define UP_NETWORK_SIZE 100


int up_network_gameplay_recive(void *arg);
int up_network_account_recive(void *arg);
void up_network_sendHeartbeat(struct up_network_datapipe *socket_data);

// warning:
// this function copy the data from source into destination,
// this function is pure evil and copy raw byte data of element_size,
// thid function do not check if destination and source is pointing to valid data
// and it do not do any bound checking
static void generic_copyElement(unsigned int element_size,unsigned char *destination,unsigned char *source)
{
    unsigned char *end = destination + element_size;
    for (; destination < end; destination++,source++) {
        *destination = *source;
    }
}

unsigned int up_copyObjectIntoBuffer(struct objUpdateInformation *object,unsigned char *buffer)
{
    unsigned int data_len = sizeof(object->data);
    data_len = (data_len > object->length) ? object->length : data_len;
    generic_copyElement(data_len,buffer,(unsigned char *)&object->data);
    return data_len;
}

unsigned int  up_copyBufferIntoObject(unsigned char *buffer,struct objUpdateInformation *object)
{
    unsigned int data_len = sizeof(object->data);
    data_len = (data_len > object->length) ? object->length : data_len;
    generic_copyElement(data_len,(unsigned char *)&object->data,buffer);
    return data_len;
}




static struct up_network_datapipe *up_network_start_setup(int port)                               // used to be main
{
    struct up_network_datapipe *p = malloc(sizeof(struct up_network_datapipe));
    if (p == NULL) {
        UP_ERROR_MSG("network setup failed malloc");
        return NULL;
    }
    //UDPsocket client_socket;                                // deklaratons of sdl udp sockets for later use
    //UDPpacket *recive_packet;
    //IPaddress addr;
    //SDL_Thread *thread;
    char file[20]="ip_address";
    char ip_address[20];
    int success;
    
    if (up_concurrentQueue_start_setup() == 0) {
        UP_ERROR_MSG("queue startup failed");
    }
                                                            //need to be started early to make sure that it is up and running before the main game loop begins
    struct up_thread_queue *queue = up_concurrentQueue_new();
    if(queue == NULL)                 //recived queu buffert so that important coordinates does not rewrites
    {
        UP_ERROR_MSG("failed queue startup");
    }
    else{
        printf("Success queue");
    }
    p->queue = queue;
    
    FILE *fp=fopen(file,"r");                               // open file, read the content, scans the file and push it into the array
    if (fp == NULL) {
        UP_ERROR_MSG_STR("Missing file, ip_address,network failure",file);
        strcpy(ip_address, "127.0.0.1");                    // this make sure the we have a fallback address, the loopback
    }else
    {
        fscanf(fp,"%s", ip_address);
        fclose(fp);
    }
    printf("%s", ip_address);

                                                            // initialize of sdlnet_init, returns -1 if not success else success
    
    success=SDLNet_Init();
    if(success==-1){
        printf("Error: Initialize not success");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Initialize success");
    }
                                                            // randomize a port so that the client can communicate, returns 0 if not success else success
    
    p->udpSocket =SDLNet_UDP_Open(0);
    if(!p->udpSocket){
        printf("Error: Could not open socket");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Socket port: ");
    }
    
                                                            // resolving conection between host and destination address, sending data throuh port 5001
    
    success=SDLNet_ResolveHost(&p->addr, ip_address, port);
    if(success==-1){
        printf("Error: Could not fill out ip address");
    }
    else{
        printf("Success filling out the ip address\n");
    }
    
    p->online = 1;
    p->sendPacket =   SDLNet_AllocPacket(1024);                                                      // allocate and resize a single udp packet
    
                                                            // creates thread for the server
    //p->recive_thread=SDL_CreateThread(up_network_recive,"up_network_recive",p);
    
    
    
    return p;
}

struct up_network_datapipe *up_network_start_gameplay_setup()
{
    struct up_network_datapipe * server_pipe = up_network_start_setup(22422);
    server_pipe->recive_thread = SDL_CreateThread(up_network_gameplay_recive,"up_network_gameplay_recive",server_pipe);
    return server_pipe;
}

struct up_network_datapipe *up_network_start_account_setup()
{
    struct up_network_datapipe * server_pipe = up_network_start_setup(44244);
    server_pipe->recive_thread = SDL_CreateThread(up_network_account_recive,"up_network_account_recive",server_pipe);
    return server_pipe;
}


void up_network_shutdown_deinit(struct up_network_datapipe *p)
{
    p->online = 0;
    SDL_Delay(100); // make sure that the recive thread has ended before we shut down all other stuff
    SDLNet_FreePacket(p->sendPacket);
    
    SDL_WaitThread(p->recive_thread, NULL);
    
    up_concurrentQueue_shutdown_deinit();
    
   
}

static int up_network_updateShipUnit(struct up_actionState *states,struct up_packet_movement *movment, int playerId)
{
    
    if (states->objectID.idx == playerId) {
        states->objectID.idx = 0; // index special means no update
        return 1;
    }
    
    struct up_objectInfo *tmpObject = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);
    if (tmpObject == NULL) {
        printf("\nRecive packet coruppted");
        return 0;
    }
    
    // TODO: timedalation
    // this is a temporary solution
    tmpObject->pos = movment->pos;
    tmpObject->speed = movment->speed;
    tmpObject->angle = movment->angle;
    tmpObject->bankAngle = movment->bankangle;
    //objUpdate[i].id;
    return 1;
}

static int up_network_updateObject(struct up_packet_movement *movment)
{
    struct up_objectInfo *tmpObject = up_unit_objAtIndex(movment->objectID.type, movment->objectID.idx);
    if (tmpObject == NULL) {
        printf("\nRecive packet coruppted");
        return 0;
    }
    
    // TODO: timedalation
    // this is a temporary solution
    tmpObject->pos = movment->pos;
    tmpObject->speed = movment->speed;
    tmpObject->angle = movment->angle;
    tmpObject->bankAngle = movment->bankangle;
    //objUpdate[i].id;
    return 1;
}

#define UP_OBJECT_BUFFER_READ_LENGTH 60



int up_network_getNewMovement(struct up_actionState *states,int max,int playerId,struct up_network_datapipe *socket_data)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;
    

    int packet_read = up_readNetworkDatabuffer(socket_data->queue,objUpdate, max);

    struct up_actionState tmp_states = {0};
    struct up_packet_movement movment = {0};
    
    int i = 0;
    int timestamp = 0;

    int success = 0;
    
    for (i = 0; i < packet_read; i++) {
        
        switch (objUpdate[i].data[0]) {
            case  UP_PACKET_ACTION_FLAG:
                up_network_action_packetDecode(&objUpdate[i], &tmp_states, &movment, &timestamp);
                //printf("server pimg ms: %d\n",(SDL_GetTicks() - timestamp));
                success = up_network_updateShipUnit(&tmp_states,&movment,playerId);
                if (success) {
                    states[tmp_states.objectID.idx] = tmp_states;
                }
                break;
            case UP_PACKET_OBJECTMOVE_FLAG:
                up_network_objectmove_packetDecode(&objUpdate[i], &movment, &timestamp);
                up_network_updateObject(&movment);
                break;
            case UP_PACKET_HEARTBEAT_FLAG:
                up_network_sendHeartbeat(socket_data);
                break;
            default:
                break;
        }
    }
    return max; //check all player slots
    
}





void up_network_sendNewMovement(struct up_actionState *states, struct up_network_datapipe *socket_data)
{
    struct up_objectInfo *object = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);
    if (object == NULL) {
        printf("send packet corrupted");
        return;
    }
    struct objUpdateInformation updateobject;
    int timestamp = SDL_GetTicks();
    //printf("send timestamp: %d\n",timestamp);
    int len = up_network_action_packetEncode(&updateobject, states, object->pos, object->speed, object->angle, object->bankAngle, timestamp);
    updateobject.length = len;
    
    UDPsocket socket = socket_data->udpSocket;
    UDPpacket *packet = socket_data->sendPacket;
    packet->address.host = socket_data->addr.host;
    packet->address.port = socket_data->addr.port;
    
    packet->len = up_copyObjectIntoBuffer(&updateobject, packet->data);
    SDLNet_UDP_Send(socket, -1, packet);
    
}

void up_network_sendHeartbeat(struct up_network_datapipe *socket_data)
{
   
    UDPsocket socket = socket_data->udpSocket;
    UDPpacket *packet = socket_data->sendPacket;
    packet->address.host = socket_data->addr.host;
    packet->address.port = socket_data->addr.port;
    
    packet->len = up_network_heartbeat_packetEncode(packet->data, 11);
    SDLNet_UDP_Send(socket, -1, packet);
    
}


int up_network_gameplay_recive(void *arg)
{
    struct up_network_datapipe *p=(struct up_network_datapipe *)arg;
   
    UDPsocket socket = p->udpSocket;
    UDPpacket *packet = SDLNet_AllocPacket(1024);
    struct objUpdateInformation local_data = {0};
    struct up_thread_queue *queue = p->queue;
    packet->address.host = p->addr.host;
    packet->address.port = p->addr.port;
    
    unsigned int len = 0;
    
    while(p->online){
        SDL_Delay(1);

        if (SDLNet_UDP_Recv(socket,packet)){
            //printf("\n pack recv: ");
            len = packet->len;
            if ((3 <= len ) && (len <= sizeof(local_data.data))) {
                //printf("pack processing len %d ", packet->len);
                local_data.id = 1; //cant be zero
                local_data.length = packet->len;
                up_copyBufferIntoObject(packet->data,&local_data);
                
                up_writeToNetworkDatabuffer(queue,&local_data);
            }
        
        }
        
        
        
        //SDL_Delay(1);

    }
    
    SDLNet_FreePacket(packet);
    return 1;
}

int up_network_account_recive(void *arg)
{
    struct up_network_datapipe *p=(struct up_network_datapipe *)arg;
    
    UDPsocket socket = p->udpSocket;
    UDPpacket *packet = SDLNet_AllocPacket(1024);
    struct objUpdateInformation local_data = {0};
    struct up_thread_queue *queue = p->queue;
    packet->address.host = p->addr.host;
    packet->address.port = p->addr.port;
    
    unsigned int len = 0;
    
    while(p->online){
        SDL_Delay(1);
        
        if (SDLNet_UDP_Recv(socket,packet)){
            //printf("\n pack recv: ");
            len = packet->len;
            if ((3 <= len ) && (len <= sizeof(local_data.data))) {
                //printf("pack processing len %d ", packet->len);
                local_data.id = 1; //cant be zero
                local_data.length = packet->len;
                up_copyBufferIntoObject(packet->data,&local_data);
                
                up_writeToNetworkDatabuffer(queue,&local_data);
            }
            
        }
        
        
        
        //SDL_Delay(1);
        
    }
    
    SDLNet_FreePacket(packet);
    return 1;
}

#define UP_USERPASS 21

int up_network_getAccountData(struct up_network_account_data *data,int max,struct up_network_datapipe *socket_data)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;
    
    
    int packet_read = up_readNetworkDatabuffer(socket_data->queue,objUpdate, max);
    

    int i = 0;
    
    for (i = 0; i < packet_read; i++) {
        
        switch (objUpdate[i].data[0]) {
            case  UP_LOGIN_FLAG:
                // do stuff, like decode packet and store it in data
                
                break;
            case UP_MAP_DATA_FLAG:
                // do stuff, and so on
                break;
            case UP_PACKET_HEARTBEAT_FLAG:  // keep this, so we dont lose the connection
                up_network_sendHeartbeat(socket_data);
                break;
            default:
                break;
        }
    }
    return max; //check all player slots
    
}

int up_network_registerAccount(char *username, char *password,int length, struct up_network_datapipe *socket_data )
{
    int i,writeSpace = 0;
    unsigned char messageToServer[768];
    unsigned char userLength = (unsigned char) strlen(username);
    unsigned char passLength = (unsigned char) strlen(password);

    messageToServer[writeSpace]=UP_REGISTRATE_FLAG;
    writeSpace++;
    messageToServer[writeSpace]=UP_USER_PASS_FLAG;
    writeSpace++;

    messageToServer[writeSpace++]=userLength;

    for(i=0; i<userLength; i++)
    {
        messageToServer[writeSpace++]=username[i];
    }
    
    messageToServer[writeSpace++]=passLength;

    for(i=0;i<passLength;i++)
    {
        messageToServer[writeSpace++]=password[i];
    }

    messageToServer[writeSpace++]='\0';


    UDPsocket socket = socket_data->udpSocket;
    UDPpacket *packet = socket_data->sendPacket;
    packet->address.host = socket_data->addr.host;
    packet->address.port = socket_data->addr.port;
    
    generic_copyElement(writeSpace, packet->data, messageToServer);
    packet->len = writeSpace;
    SDLNet_UDP_Send(socket, -1, packet);
    return 0;

}
