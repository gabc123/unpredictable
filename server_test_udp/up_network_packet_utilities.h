//
//  up_network_packet_utilities.h
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-17.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_NETWORK_PACKET_UTILITIES_H
#define UP_NETWORK_PACKET_UTILITIES_H

#include "up_ship.h"
#include "up_vertex.h"
#include "up_thread_utilities.h"

// account flags
#define UP_REGISTRATE_FLAG (unsigned char)1
#define UP_LOGIN_FLAG (unsigned char)2
#define UP_USER_PASS_FLAG (unsigned char)4
#define REGSUCESSS (unsigned char)5
#define REGFAILED (unsigned char)6
#define LOGINSUCESS (unsigned char)7
#define LOGINFAILED (unsigned char)8


// gameplay flags
#define UP_PACKET_ACTION_FLAG (unsigned char)10
#define UP_PACKET_OBJECTMOVE_FLAG (unsigned char)11
#define UP_PACKET_PLAYER_HEALTHSTATS_FLAG (unsigned char)12
#define UP_PACKET_PLAYER_JOINED (unsigned char)14
#define UP_PACKET_REMOVE_OBJ_FLAG (unsigned char)15
#define UP_PACKET_PLAYER_STATS_FLAG (unsigned char)16
#define UP_PACKET_PLAYER_SHIPMODEL_FLAG (unsigned char)17

#define UP_PACKET_PLAYER_EXIT_FLAG (unsigned char)18

// maintenance flags
#define UP_PACKET_HEARTBEAT_FLAG (unsigned char)40
#define UP_PACKET_MAPDATA_FLAG (unsigned char)50



#define UP_USER_NAME_PASS_MAX 255

struct up_packet_player_joined
{
    struct up_objectID objectID;
    int modelId;
    struct up_vec3 pos;
    struct up_player_stats player_stats;
};

struct up_packet_movement
{
    int modelId;
    struct up_objectID objectID;
    struct up_vec3 pos;
    float speed;
    float angle;
    float bankangle;
    int timestamp;
};

/*
 All these function encode or decode a specifc packet type
 They return the length of the packet, or 0 on error.
 The data to be decoded or encoded is stored/loaded from/into the data field that is ether a
 array of unsigned char with at least 768 in length, or it will use the objUpdateInformation structure
 that contains the relevent buffer and data, (objUpdateInformation can also be sent over a interThread comunication buffer)
 */
int up_network_packet_playerExit_encode(unsigned char *data,const char *userName,int playerId);
int up_network_packet_playerExit_decode(unsigned char *data,char *userName,int *playerId);


int up_network_logInRegistrate_packetEncode(unsigned char *data,int clientId, unsigned char regLogFlag);

int up_intercom_packet_playerJoind_encode(unsigned char *data,struct up_packet_player_joined *player);
int up_intercom_packet_playerJoind_decode(unsigned char *data,struct up_packet_player_joined *player);

int up_network_packet_changModelEncode(unsigned char *data,int modelId,int playerId,int timestamp);
int up_network_packet_changModelDecode(unsigned char *data,int *modelId,int *playerId,int *timestamp);

int up_network_packet_mapData_encode(unsigned char *data,int playerIndex,int mapSeed,int numPlayersOnline);
int up_network_packet_mapData_decode(unsigned char *data,int *playerIndex,int *mapSeed,int *numPlayersOnline);


int up_network_heartbeat_packetEncode(unsigned char *data,int timestamp);
int up_network_heartbeat_packetDecode(unsigned char *data,int *timestamp);

int up_network_playerStats_packetEncode(struct objUpdateInformation *object,struct up_player_stats *player,int timestamp);
int up_network_playerStats_packetDecode(struct objUpdateInformation *object,struct up_player_stats *player,int *timestamp);

int up_network_removeObj_packetEncode(struct objUpdateInformation *object,struct up_objectID objId,int timestamp);
int up_network_removeObj_packetDecode(struct objUpdateInformation *object,struct up_objectID *objId,int *timestamp);

int up_network_objectmove_packetEncode(struct objUpdateInformation *object,
                                       struct up_objectID objId, int modelId,
                                       struct up_vec3 pos,float speed,
                                       float angle,float bankangle,int timestamp);

int up_network_objectmove_packetDecode(struct objUpdateInformation *object,
                                       struct up_packet_movement *movement,
                                       int *timestamp);


int up_network_action_packetEncode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 pos,float speed,
                                   float angle,float bankangle,int modelId,int timestamp);

int up_network_action_packetDecode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_packet_movement *movement,
                                   int *timestamp);

// warning:
// this function copy the data from source into destination,
// this function is pure evil and copy raw byte data of element_size,
// thid function do not check if destination and source is pointing to valid data
// and it do not do any bound checking
void generic_copyElement(unsigned int element_size,unsigned char *destination,unsigned char *source);

#endif /* defined(__up_server_part_xcode__up_network_packet_utilities__) */
