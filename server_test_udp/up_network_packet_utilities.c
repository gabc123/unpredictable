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
static void generic_copyElement(unsigned int element_size,unsigned char *destination,unsigned char *source)
{
    unsigned char *end = destination + element_size;
    for (; destination < end; destination++,source++) {
        *destination = *source;
    }
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

static int packet_loacationMovement_decode(unsigned char *data,struct up_vec3 *pos,
                                           float *speed,float *angle,float *bankangle)
{
    int read_pos = 0;
    int element_len = sizeof(float);
    // we need to copy each component by them self , due to padding and compiler stuff
    // this asumes x86 prosessor so we do not need to worry about big/little endines
    
    // position data
    generic_copyElement(element_len, (unsigned char *)&pos->x, &data[read_pos]);
    read_pos +=element_len;
    
    generic_copyElement(element_len, (unsigned char *)&pos->y, &data[read_pos]);
    read_pos +=element_len;
    
    generic_copyElement(element_len, (unsigned char *)&pos->z, &data[read_pos]);
    read_pos +=element_len;
    
    // speed
    generic_copyElement(element_len, (unsigned char *)speed, &data[read_pos]);
    read_pos +=element_len;
    
    // angles (used to compute direction)
    generic_copyElement(element_len, (unsigned char *)angle, &data[read_pos]);
    read_pos +=element_len;
    
    // bank angles (used to compute direction)
    generic_copyElement(element_len, (unsigned char *)bankangle, &data[read_pos]);
    read_pos +=element_len;
    
    return read_pos; // returns the total length that has been transferd into data
}


int up_network_action_packetEncode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 pos,float speed,
                                   float angle,float bankangle,int timestamp)
{
    int read_pos = 0;
    
    
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
                                   struct up_vec3 *pos,float *speed,
                                   float *angle,float *bankangle,int *timestamp)
{
    int read_pos = 0;
    
    
    // store the index/id and type
    int index_len = sizeof(action->objectID.idx);
    object->id = action->objectID.idx;
    generic_copyElement(index_len,(unsigned char *)&action->objectID.idx, &object->data[read_pos]);
    read_pos += index_len;
    
    action->objectID.type = object->data[read_pos];
    read_pos++;
    
    // action state data
    action->fireWeapon.state = object->data[read_pos];
    read_pos++;
    
    action->engine.state = object->data[read_pos];
    read_pos++;
    
    action->maneuver.state = object->data[read_pos];
    read_pos++;
    
    read_pos += packet_loacationMovement_decode(&object->data[read_pos], pos, speed, angle, bankangle);
    
    int timestamp_len = sizeof(timestamp);
    generic_copyElement(timestamp_len, (unsigned char *)&timestamp,&object->data[read_pos]);
    read_pos += timestamp_len;
    
    //object->length = read_pos;
    return read_pos; //return length of packet
}
