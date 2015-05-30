//
//  up_network_module.c
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_game_communication.h"
#include "up_modelRepresentation.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "up_thread_utilities.h"
#include "up_utilities.h"
#include "up_error.h"
#include "up_network_packet_utilities.h"
#include "sha256.h"

#define UP_NETWORK_SIZE 100




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



int up_game_communication_get(struct up_actionState *states,int max,int playerId,struct up_interThread_communication *pipe)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;
    

    int packet_read = up_readNetworkDatabuffer(pipe->simulation_input,objUpdate, max);

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
            default:
                break;
        }
    }
    return max; //check all player slots
    
}





void up_game_communication_sendAction(struct up_actionState *states,int numActions,struct up_interThread_communication *pipe)
{
    struct up_objectInfo *object = NULL;
    
    struct objUpdateInformation updateobject = {0};
    int len  = 0;
    int timestamp = 0;
    int i  =0 ;
    for (i=0; i < numActions; i++) {
        object = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);
        if (object == NULL) {
            printf("send packet corrupted");
            continue;
        }
        
        timestamp = up_clock_ms();
        len = up_network_action_packetEncode(&updateobject, states, object->pos, object->speed, object->angle, object->bankAngle, timestamp);
        if (len > 0) {
            updateobject.length = len;
            up_writeToNetworkDatabuffer(pipe->simulation_output, &updateobject);
        }
    }
    
}


int up_network_getAccountData(struct up_network_account_data *data,int max,struct up_interThread_communication *pipe)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;
    
    
    int packet_read = up_readNetworkDatabuffer(pipe->simulation_input,objUpdate, max);
    
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
                //up_network_sendHeartbeat(socket_data);
                data[i].noResponse = 1;
                break;
            default:
                break;
        }
    }
    return max; //check all player slots
    
}


