//
//  up_network_packet_utilities.c
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-17.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_network_packet_utilities.h"

#include "up_error.h"
#include "up_ship.h"
#include "up_vertex.h"
#include "up_server.h"
#include "up_thread_utilities.h"

// warning:
// this function copy the data from source into destination,
// this function is pure evil and copy raw byte data of element_size,
// thid function do not check if destination and source is pointing to valid data
// and it do not do any bound checking
void generic_copyElement(unsigned int element_size,unsigned char *destination,unsigned char *source)
{
    unsigned char *end = destination + element_size;
    for (; destination < end; destination++,source++) {
        *destination = *source;
    }
}


int up_intercom_packet_playerJoind_encode(unsigned char *data,struct up_packet_player_joined *player)
{
    int read_pos = 0;
    data[read_pos] = UP_PACKET_PLAYER_JOINED;
    read_pos++;
    
    // becouse this is internal communication we do not need to worry about padding and stuff
    // resulting in that we do not need to pack each element
    unsigned int playerJoindSize = sizeof(*player);

    generic_copyElement(playerJoindSize, &data[read_pos], (unsigned char *)player);
    read_pos += playerJoindSize;
    
    return read_pos;
}

int up_intercom_packet_playerJoind_decode(unsigned char *data,struct up_packet_player_joined *player)
{
    int read_pos = 0;
    if (data[read_pos] != UP_PACKET_PLAYER_JOINED) {
        return 0;
    }
    read_pos++;
    
    // becouse this is internal communication we do not need to worry about padding and stuff
    // resulting in that we do not need to pack each element
    unsigned int playerJoindSize = sizeof(*player);
    
    generic_copyElement(playerJoindSize, (unsigned char *)player, &data[read_pos]);
    read_pos += playerJoindSize;
    
    return read_pos;
}



int up_network_logInRegistrate_packetEncode(unsigned char *data,int clientId, unsigned char regLogFlag)
{
    int read_pos = 0;
    data[read_pos] = regLogFlag;
    read_pos++;
    int clintIdSize = sizeof(clientId);
    generic_copyElement(clintIdSize, &data[read_pos],(unsigned char *)&clientId);
    
    read_pos+=clintIdSize;
    return read_pos;
}

int up_network_logInRegistrate_packetDecode(unsigned char *data,int *clientId, unsigned char *regLogFlag)
{
    int read_pos = 0;
    *regLogFlag = data[read_pos];
    
    read_pos++;
    int clintIdSize = sizeof(*clientId);
    generic_copyElement(clintIdSize, (unsigned char *) clientId, &data[read_pos]);
    
    read_pos+=clintIdSize;
    return read_pos;
}

int up_network_packet_mapData_encode(unsigned char *data,int playerIndex,int mapSeed,int numPlayersOnline)
{
    int read_pos = 0;
    data[read_pos] = UP_PACKET_MAPDATA_FLAG;
    read_pos++;
    
    unsigned int playerIndexSize = sizeof(playerIndex);
    generic_copyElement(playerIndexSize, &data[read_pos], (unsigned char *)&playerIndex);
    read_pos += playerIndexSize;
    
    unsigned int mapSeedSize = sizeof(mapSeed);
    generic_copyElement(mapSeedSize, &data[read_pos], (unsigned char *)&mapSeed);
    read_pos += mapSeedSize;
    
    unsigned int numPlayersOnlinedSize = sizeof(numPlayersOnline);
    generic_copyElement(numPlayersOnlinedSize, &data[read_pos], (unsigned char *)&numPlayersOnline);
    read_pos += numPlayersOnlinedSize;
    
    return read_pos;
}

int up_network_packet_mapData_decode(unsigned char *data,int *playerIndex,int *mapSeed,int *numPlayersOnline)
{
    int read_pos = 0;
    if (data[read_pos] != UP_PACKET_MAPDATA_FLAG) {
        return 0;
    }
    read_pos++;
    
    unsigned int playerIndexSize = sizeof(*playerIndex);
    generic_copyElement(playerIndexSize,(unsigned char *)playerIndex,&data[read_pos]);
    read_pos += playerIndexSize;
    
    unsigned int mapSeedSize = sizeof(*mapSeed);
    generic_copyElement(mapSeedSize,(unsigned char *)mapSeed,&data[read_pos]);
    read_pos += mapSeedSize;
    
    unsigned int numPlayersOnlinedSize = sizeof(*numPlayersOnline);
    generic_copyElement(numPlayersOnlinedSize,(unsigned char *)numPlayersOnline,&data[read_pos]);
    read_pos += numPlayersOnlinedSize;
    
    return read_pos;
}


int up_network_heartbeat_packetEncode(unsigned char *data,int timestamp)
{
    int read_pos = 0;
    data[read_pos] = UP_PACKET_HEARTBEAT_FLAG;
    read_pos++;
    
    int timestamp_len = sizeof(timestamp);
    generic_copyElement(timestamp_len,&data[read_pos],(unsigned char *)&timestamp);
    read_pos += timestamp_len;
    
    return read_pos; //return length of packet
}

int up_network_heartbeat_packetDecode(unsigned char *data,int *timestamp)
{
    int read_pos = 0;
    if (data[read_pos] !=  UP_PACKET_HEARTBEAT_FLAG) {
        return 0;
    }
    read_pos++;
    
    int timestamp_len = sizeof(*timestamp);
    generic_copyElement(timestamp_len, (unsigned char *)timestamp,&data[read_pos]);
    read_pos += timestamp_len;
    
    return read_pos; //return length of packet
}


static int packet_loacationMovement_encode(unsigned char *data,struct up_vec3 pos,
                                           float speed,float angle,float bankangle)
{
    int read_pos = 0;
    int element_len = sizeof(float);
    // we need to copy each component by them self , due to padding and compiler stuff
    // this asumes x86 prosessor so we do not need to worry about big/little endines
    
    // position data
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&pos.x);
    read_pos +=element_len;
    
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&pos.y);
    read_pos +=element_len;
    
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&pos.z);
    read_pos +=element_len;
    
    // speed
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&speed);
    read_pos +=element_len;
   
    // angles (used to compute direction)
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&angle);
    read_pos +=element_len;
    
    // bank angles (used to compute direction)
    generic_copyElement(element_len, &data[read_pos], (unsigned char *)&bankangle);
    read_pos +=element_len;
    
    return read_pos; // returns the total length that has been transferd into data
}

static int packet_loacationMovement_decode(unsigned char *data,struct up_packet_movement *movement)
{
    int read_pos = 0;
    int element_len = sizeof(float);
    // we need to copy each component by them self , due to padding and compiler stuff
    // this asumes x86 prosessor so we do not need to worry about big/little endines
    
    // position data
    generic_copyElement(element_len, (unsigned char *)&movement->pos.x, &data[read_pos]);
    read_pos +=element_len;
    
    generic_copyElement(element_len, (unsigned char *)&movement->pos.y, &data[read_pos]);
    read_pos +=element_len;
    
    generic_copyElement(element_len, (unsigned char *)&movement->pos.z, &data[read_pos]);
    read_pos +=element_len;
    
    // speed
    generic_copyElement(element_len, (unsigned char *)&movement->speed, &data[read_pos]);
    read_pos +=element_len;
    
    // angles (used to compute direction)
    generic_copyElement(element_len, (unsigned char *)&movement->angle, &data[read_pos]);
    read_pos +=element_len;
    
    // bank angles (used to compute direction)
    generic_copyElement(element_len, (unsigned char *)&movement->bankangle, &data[read_pos]);
    read_pos +=element_len;
    
    return read_pos; // returns the total length that has been transferd into data
}


int up_network_objectmove_packetEncode(struct objUpdateInformation *object,
                                       struct up_objectID objId,
                                       struct up_vec3 pos,float speed,
                                       float angle,float bankangle,int timestamp)
{
    int read_pos = 0;
    object->data[read_pos] = UP_PACKET_OBJECTMOVE_FLAG;
    read_pos++;
    
    int index_len = sizeof(objId.idx);
    object->id = objId.idx;
    generic_copyElement(index_len, &object->data[read_pos],(unsigned char *)&objId.idx);
    read_pos += index_len;
    
    object->data[read_pos] = (unsigned char)objId.type;
    read_pos++;
    
    read_pos += packet_loacationMovement_encode(&object->data[read_pos], pos, speed, angle, bankangle);
    
    int timestamp_len = sizeof(timestamp);
    generic_copyElement(timestamp_len, &object->data[read_pos],(unsigned char *)&timestamp);
    read_pos += timestamp_len;
    
    object->length = read_pos;
    return read_pos; //return length of packet
}

int up_network_objectmove_packetDecode(struct objUpdateInformation *object,
                                   struct up_packet_movement *movement,
                                   int *timestamp)
{
    int read_pos = 0;
    if (object->data[read_pos] !=  UP_PACKET_ACTION_FLAG) {
        return 0;
    }
    read_pos++;
    
    // store the index/id and type
    int index_len = sizeof(movement->objectID.idx);
    
    generic_copyElement(index_len,(unsigned char *)&movement->objectID.idx, &object->data[read_pos]);
    read_pos += index_len;
    object->id = movement->objectID.idx;

    
    movement->objectID.type = object->data[read_pos];
    read_pos++;

    read_pos += packet_loacationMovement_decode(&object->data[read_pos], movement);
    
    int timestamp_len = sizeof(*timestamp);
    generic_copyElement(timestamp_len, (unsigned char *)timestamp,&object->data[read_pos]);
    read_pos += timestamp_len;
    movement->timestamp = *timestamp;
    
    return read_pos; //return length of packet
}




int up_network_action_packetEncode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 pos,float speed,
                                   float angle,float bankangle,int timestamp)
{
    int read_pos = 0;
    object->data[read_pos] = UP_PACKET_ACTION_FLAG;
    read_pos++;
    
    // store the index/id and type
    int index_len = sizeof(action->objectID.idx);
    object->id = action->objectID.idx;
    generic_copyElement(index_len, &object->data[read_pos],(unsigned char *)&action->objectID.idx);
    read_pos += index_len;
    
    object->data[read_pos] = (unsigned char)action->objectID.type;
    read_pos++;
    
    // action state data
    object->data[read_pos] = (unsigned char)action->fireWeapon.state;
    read_pos++;
    
    object->data[read_pos] = (unsigned char)action->engine.state;
    read_pos++;
    
    object->data[read_pos] = (unsigned char)action->maneuver.state;
    read_pos++;
    
    read_pos += packet_loacationMovement_encode(&object->data[read_pos], pos, speed, angle, bankangle);
    
    int timestamp_len = sizeof(timestamp);
    generic_copyElement(timestamp_len, &object->data[read_pos],(unsigned char *)&timestamp);
    read_pos += timestamp_len;
    
    object->length = read_pos;
    return read_pos; //return length of packet
}


int up_network_action_packetDecode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_packet_movement *movement,
                                   int *timestamp)
{
    int read_pos = 0;
    if (object->data[read_pos] !=  UP_PACKET_ACTION_FLAG) {
        return 0;
    }
    read_pos++;
    
    // store the index/id and type
    int index_len = sizeof(action->objectID.idx);
    
    generic_copyElement(index_len,(unsigned char *)&action->objectID.idx, &object->data[read_pos]);
    read_pos += index_len;
    object->id = action->objectID.idx;
    movement->objectID.idx = action->objectID.idx;
    
    action->objectID.type = object->data[read_pos];
    read_pos++;
    movement->objectID.type = action->objectID.type;
    
    
    // action state data
    action->fireWeapon.state = object->data[read_pos];
    read_pos++;
    
    action->engine.state = object->data[read_pos];
    read_pos++;
    
    action->maneuver.state = object->data[read_pos];
    read_pos++;
    
    read_pos += packet_loacationMovement_decode(&object->data[read_pos], movement);
    
    int timestamp_len = sizeof(*timestamp);
    generic_copyElement(timestamp_len, (unsigned char *)timestamp,&object->data[read_pos]);
    read_pos += timestamp_len;
    movement->timestamp = *timestamp;
    
    //object->length = read_pos;
    return read_pos; //return length of packet
}
