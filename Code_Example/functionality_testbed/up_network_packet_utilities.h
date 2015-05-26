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
#define REGSUCESSS (unsigned char) 5
#define REGFAILED (unsigned char) 6
#define LOGINSUCESS (unsigned char) 7
#define LOGINFAILED (unsigned char) 8


// gameplay flags
#define UP_PACKET_ACTION_FLAG (unsigned char)10
#define UP_PACKET_OBJECTMOVE_FLAG (unsigned char)11
#define UP_PACKET_PLAYER_HEALTHSTATS_FLAG (unsigned char)12

// maintenance flags
#define UP_PACKET_HEARTBEAT_FLAG (unsigned char)40
#define UP_MAP_DATA_FLAG (unsigned char)50


struct up_packet_movement
{
    struct up_objectID objectID;
    struct up_vec3 pos;
    float speed;
    float angle;
    float bankangle;
    int timestamp;
};




int up_network_heartbeat_packetEncode(unsigned char *data,int timestamp);
int up_network_heartbeat_packetDecode(unsigned char *data,int *timestamp);


int up_network_objectmove_packetEncode(struct objUpdateInformation *object,
                                       struct up_objectID objId,
                                       struct up_vec3 pos,float speed,
                                       float angle,float bankangle,int timestamp);

int up_network_objectmove_packetDecode(struct objUpdateInformation *object,
                                       struct up_packet_movement *movement,
                                       int *timestamp);


int up_network_action_packetEncode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 pos,float speed,
                                   float angle,float bankangle,int timestamp);

int up_network_action_packetDecode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_packet_movement *movement,
                                   int *timestamp);

#endif /* defined(__up_server_part_xcode__up_network_packet_utilities__) */
