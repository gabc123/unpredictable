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
#include <math.h>
#include "up_thread_utilities.h"
#include "up_error.h"
#include "up_network_packet_utilities.h"
#include "sha256.h"

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
    //printf("copybuffer data_len:%d ",data_len);
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
    /* // already done in the main startup sequnce
    success=SDLNet_Init();
    if(success==-1){
        printf("Error: Initialize not success");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Initialize success");
    }*/
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
    p->sendPacket =   SDLNet_AllocPacket(1024);
    if (!p->sendPacket) {
        UP_ERROR_MSG_STR("sdlnet allocpacket error", SDLNet_GetError());
    }
    // allocate and resize a single udp packet

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
printf("Account ending connection closed\n");
    SDL_Delay(100); // make sure that the recive thread has ended before we shut down all other stuff
    SDL_WaitThread(p->recive_thread, NULL);

    SDLNet_FreePacket(p->sendPacket);

    up_concurrentQueue_shutdown_deinit();


}

static int up_network_updateShipUnit(struct up_actionState *states,struct up_packet_movement *movement, int playerId)
{

    
    struct up_objectInfo fallback = {0};
    struct up_objectInfo *tmpObject = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);
    if (tmpObject == NULL) {
        fallback = up_asset_createObjFromId(movement->modelId);
        up_server_unit_setObjAtindex(states->objectID.type, fallback, states->objectID.idx);
        tmpObject = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);

        if (tmpObject == NULL) {
            printf("\nRecive packet coruppted");
            return 0;
        }
    }

    struct up_vec3 dir = {0};
    dir.x = sinf(movement->angle);
    dir.y = cosf(movement->angle);

    // TODO: timedalation
    // this is a temporary solution
    tmpObject->modelId = movement->modelId;
    float distance = up_distance(tmpObject->pos, movement->pos) ;
    
    if (states->objectID.idx == playerId && distance < 20) {
        states->objectID.idx = 0; // index special means no update
     
        return 1;
    }
    
    if ( distance > 4) {
        tmpObject->pos.x = (tmpObject->pos.x + movement->pos.x)/2;
        tmpObject->pos.y = (tmpObject->pos.y + movement->pos.y)/2;
        tmpObject->pos.z = (tmpObject->pos.z + movement->pos.z)/2;

    }
    
    tmpObject->dir = dir;
    tmpObject->speed = movement->speed;
    tmpObject->angle = movement->angle;
    tmpObject->bankAngle = movement->bankangle;
    //objUpdate[i].id;
    return 1;
}

static int up_network_updateObject(struct up_packet_movement *movement)
{
    struct up_vec3 dir = {0};
    dir.x = sinf(movement->angle);
    dir.y = cosf(movement->angle);

    struct up_objectInfo obj_tmp = {0};

    struct up_objectInfo *tmpObject = up_unit_objAtIndex(movement->objectID.type, movement->objectID.idx);
    if (tmpObject == NULL) {
        obj_tmp = up_asset_createObjFromId(movement->modelId);
        obj_tmp.pos = movement->pos;
        obj_tmp.dir = dir;
        obj_tmp.speed = movement->speed;
        obj_tmp.angle = movement->angle;
        obj_tmp.bankAngle = movement->bankangle;
        obj_tmp.objectId = movement->objectID;

        up_server_unit_setObjAtindex(movement->objectID.type, obj_tmp, movement->objectID.idx);
        tmpObject = up_unit_objAtIndex(movement->objectID.type, movement->objectID.idx);
        return 1;
    }

    // TODO: timedalation
    // this is a temporary solution
    tmpObject->modelId = movement->modelId;
    tmpObject->pos = (up_distance(tmpObject->pos, movement->pos) < 4.0) ? tmpObject->pos : movement->pos;
    tmpObject->dir = dir;
    tmpObject->speed = movement->speed;
    tmpObject->angle = movement->angle;
    tmpObject->bankAngle = movement->bankangle;
    //objUpdate[i].id;
    return 1;
}

#define UP_OBJECT_BUFFER_READ_LENGTH 60

static void transfer_inventory(struct up_player_stats *inventoryA,struct up_player_stats *inventoryB)
{
    inventoryA->health.current = inventoryB->health.current;
    inventoryA->armor.current = inventoryB->armor.current;
    inventoryA->missile.current = inventoryB->missile.current;
    inventoryA->bullets.current = inventoryB->bullets.current;
    inventoryA->laser.current = inventoryB->laser.current;

}

int up_network_getNewStates(struct up_actionState *states,int max,int playerId,struct up_player_stats *player_inventory,struct up_network_datapipe *socket_data)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;


    int packet_read = up_readNetworkDatabuffer(socket_data->queue,objUpdate, UP_OBJECT_BUFFER_READ_LENGTH);

    struct up_actionState tmp_states = {0};
    struct up_packet_movement movment = {0};
    struct up_objectInfo *tmp_obj = NULL;

    int i = 0;
    int timestamp = 0;

    int success = 0;

    struct up_player_stats tmp_inventory = {0};
    int modelId = 0;
    struct up_objectID tmp_objId = {0};

    for (i = 0; i < packet_read; i++) {

        switch (objUpdate[i].data[0]) {
            case  UP_PACKET_ACTION_FLAG:
                up_network_action_packetDecode(&objUpdate[i], &tmp_states, &movment, &timestamp);
                //printf("server pimg ms: %d\n",(SDL_GetTicks() - timestamp));
                success = up_network_updateShipUnit(&tmp_states,&movment,playerId);
                if (success && tmp_states.objectID.idx < max) {
                    states[tmp_states.objectID.idx] = tmp_states;
                }
                break;
            case UP_PACKET_OBJECTMOVE_FLAG:
                success = up_network_objectmove_packetDecode(&objUpdate[i], &movment, &timestamp);
                if (success > 0) {
                    up_network_updateObject(&movment);
                }

                break;
            case UP_PACKET_REMOVE_OBJ_FLAG:
                success = up_network_removeObj_packetDecode(&objUpdate[i], &tmp_objId, &timestamp);
                if (success > 0 && tmp_objId.idx != 0) {
                    if (tmp_objId.idx < max && tmp_objId.type == up_ship_type) {
                        states[tmp_states.objectID.idx].objectID.idx = 0;   //to indicate a player has left
                    }
                    up_unit_remove(tmp_objId.type, tmp_objId.idx);
                }
                break;
            case UP_PACKET_PLAYER_STATS_FLAG:
                success = up_network_playerStats_packetDecode(&objUpdate[i], &modelId, &tmp_objId, &tmp_inventory, &timestamp);
                if (success > 0 && tmp_objId.idx != 0) {
                    tmp_obj = up_unit_objAtIndex(tmp_objId.type, tmp_objId.idx);
                    if (tmp_obj == NULL) {
                        continue;
                    }
                    if (playerId == tmp_obj->objectId.idx) {
                        tmp_obj->modelId = modelId;
                        transfer_inventory(player_inventory, &tmp_inventory);
                    }
                }
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



void up_network_sendChangeModel(int modelId,int playerId, struct up_network_datapipe *socket_data)
{
    struct objUpdateInformation updateobject;
    int timestamp = SDL_GetTicks();
    //printf("send timestamp: %d\n",timestamp);
    int len = up_network_packet_changModelEncode(&updateobject.data[0], modelId, playerId, timestamp);
    updateobject.length = len;

    UDPsocket socket = socket_data->udpSocket;
    UDPpacket *packet = socket_data->sendPacket;
    packet->address.host = socket_data->addr.host;
    packet->address.port = socket_data->addr.port;

    packet->len = up_copyObjectIntoBuffer(&updateobject, packet->data);
    SDLNet_UDP_Send(socket, -1, packet);

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
    int len = up_network_action_packetEncode(&updateobject, states, object->pos, object->speed, object->angle, object->bankAngle, object->modelId, timestamp);
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
    UDPpacket *packet = SDLNet_AllocPacket(512);
    struct objUpdateInformation local_data = {0};
    struct up_thread_queue *queue = p->queue;
    packet->address.host = p->addr.host;
    packet->address.port = p->addr.port;

    int len = 0;

    while(p->online){
        SDL_Delay(1);

        if (SDLNet_UDP_Recv(socket,packet)){
            //printf("\n pack recv: ");
            len = packet->len;
            if ((2 <= len ) && (len <= sizeof(local_data.data))) {
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
    if (!packet) {
        UP_ERROR_MSG_STR("Acount thread allocpacket error", SDLNet_GetError());
    }

    struct objUpdateInformation local_data = {0};
    struct up_thread_queue *queue = p->queue;
    packet->address.host = p->addr.host;
    packet->address.port = p->addr.port;

    unsigned int len = 0;

    printf("\nAccount recive thread started\n");

    while(p->online){
        SDL_Delay(1);

        if (SDLNet_UDP_Recv(socket,packet)){
            //printf("\n pack recv: ");
            len = packet->len;
            if ((3 <= len ) && (len <= sizeof(local_data.data))) {
                //printf("pack processing len %d \n", packet->len);
                local_data.id = 1; //cant be zero
                local_data.length = packet->len;
                printf(" %d",local_data.length);
                fflush(stdout);
                //up_copyBufferIntoObject(packet->data,&local_data);
                if (len >= sizeof(local_data.data)) {
                    printf("Account recive: Bad packet size\n");
                    continue;
                }
                generic_copyElement(len,local_data.data,packet->data);
                //printf(" data copy done ");
                fflush(stdout);
                up_writeToNetworkDatabuffer(queue,&local_data);
                //printf(" data to queue done ");
            }

        }



        //SDL_Delay(1);

    }
    printf("Account login recv connection closed\n");
    SDLNet_FreePacket(packet);
    return 1;
}



int up_network_getAccountData(struct up_network_account_data *data,int max,struct up_network_datapipe *socket_data)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;


    int packet_read = up_readNetworkDatabuffer(socket_data->queue,objUpdate, max);

    struct up_map_data tmp_map = {0};
    int i = 0;
    int read_pos = 0;
    for (i = 0; i < packet_read; i++) {

        switch (objUpdate[i].data[0]) {
            case  UP_LOGIN_FLAG:
                read_pos++;
                read_pos += up_network_logInRegistrate_packetDecode(&objUpdate[i].data[read_pos], &data->playerId, &data->serverResponse);

                if (data->serverResponse == LOGINSUCESS) {
                    read_pos = up_network_packet_mapData_decode(&objUpdate[i].data[read_pos], &tmp_map.playerIndex, &tmp_map.mapSeed, &tmp_map.numPlayersOnline);
                    // if the map was correctly decoded, den return it
                    data->map = (read_pos > 0) ? tmp_map : data->map;
                }

                // do stuff, like decode packet and store it in data

                break;
            case UP_REGISTRATE_FLAG:
                up_network_logInRegistrate_packetDecode(&objUpdate[i].data[1], &data[i].playerId, &data[i].serverResponse);

                // do stuff, and so on

                break;
            case UP_PACKET_HEARTBEAT_FLAG:  // keep this, so we dont lose the connection
                up_network_sendHeartbeat(socket_data);
                data[i].noResponse = 1;
                break;
            default:
                break;
        }
    }
    return max; //check all player slots

}

static int up_network_accountManagement(unsigned char flag,char *username,unsigned char userLength, char *password, struct up_network_datapipe *socket_data)
{
    int i,writeSpace = 0;
    unsigned char messageToServer[768];

    unsigned char hashedPass[SHA256_BLOCK_SIZE];

    up_hashText((char *)hashedPass,password,(int)strlen(password));
    unsigned char passLength = SHA256_BLOCK_SIZE;

    messageToServer[writeSpace]=flag;
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


int up_network_exitProg(char *username,int playerId, struct up_network_datapipe *socket_data)
{

    UDPsocket socket = socket_data->udpSocket;
    UDPpacket *packet = socket_data->sendPacket;
    packet->address.host = socket_data->addr.host;
    packet->address.port = socket_data->addr.port;

    packet->len = up_network_packet_playerExit_encode(packet->data, username, playerId);

    SDLNet_UDP_Send(socket, -1, packet);
    return 0;

}


int up_network_registerAccount(char *username, char *password, int length, struct up_network_datapipe *socket_data)
{
    unsigned char hashedPass[SHA256_BLOCK_SIZE];
    up_hashText((char *)hashedPass,password,(int)strlen(password));

    unsigned char userLength = (unsigned char) strlen(username);
    username[userLength] = '\0';
    userLength++;
    up_network_accountManagement(UP_REGISTRATE_FLAG,username,userLength,(char *)hashedPass,socket_data);
    return 0;

}

int up_network_loginAccount(char *username, char *password, int length, struct up_network_datapipe *socket_data)
{
    unsigned char hashedPass[SHA256_BLOCK_SIZE];
    printf("\nHasing password: ");
    up_hashText((char *)hashedPass,password,(int)strlen(password));
    printf("Hasing password: done \n");
    unsigned char userLength = (unsigned char) strlen(username);
    username[userLength] = '\0';
    userLength++;
    up_network_accountManagement(UP_LOGIN_FLAG,username,userLength,(char *)hashedPass,socket_data);
    return 0;

}
