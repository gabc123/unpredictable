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
#include <math.h>

//macro from bradconte.com hashing
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

#define UP_NETWORK_SIZE 100

// magnus has done most of this file


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






static int up_network_updateShipUnit(struct up_actionState *states,struct up_packet_movement *movement)
{
    
    
    struct up_objectInfo *tmpObject = up_unit_objAtIndex(states->objectID.type, states->objectID.idx);
    if (tmpObject == NULL) {
        //printf("\nRecive packet coruppted");
        return 0;
    }
    struct up_vec3 dir = {0};
    dir.x = sinf(movement->angle);
    dir.y = cosf(movement->angle);
    // TODO: timedalation
    // this is a temporary solution

    //float distance = up_distance(tmpObject->pos, movement->pos);


    tmpObject->pos = movement->pos;

    

    tmpObject->dir = dir;
    tmpObject->speed = movement->speed;
    tmpObject->angle = movement->angle;
    tmpObject->bankAngle = movement->bankangle;
    //objUpdate[i].id;
    return 1;
}



#define UP_OBJECT_BUFFER_READ_LENGTH 60

static void set_shipModel(int modelId, int index)
{
    struct up_objectInfo *tmpObject = up_unit_objAtIndex(up_ship_type, index);
    if (tmpObject == NULL) {
        //printf("\nRecive packet coruppted");
        return ;
    }
    tmpObject->modelId = modelId;
}



static struct up_packet_player_joined getPlayerExitPacket(int playerId)
{
    struct up_packet_player_joined tmp_player = {0};
    struct up_objectInfo *player = up_unit_objAtIndex(up_ship_type, playerId);
    if(player == NULL)
    {
        return tmp_player;
    }
    
    tmp_player.modelId = player->modelId;
    tmp_player.objectID = player->objectId;
    tmp_player.player_stats.health.current = 100;
    tmp_player.player_stats.armor.current = 100;
    tmp_player.player_stats.missile.current = 20;
    tmp_player.player_stats.bullets.current = 40;
    tmp_player.player_stats.laser.current = 20;
    tmp_player.pos = player->pos;
    
    return tmp_player;
}

int up_game_communication_getAction(struct up_actionState *states,int *deltaArray,int max,struct up_interThread_communication *pipe,struct up_interThread_communication *account)
{
    struct objUpdateInformation objUpdate[UP_OBJECT_BUFFER_READ_LENGTH];
    max = (max < UP_OBJECT_BUFFER_READ_LENGTH) ? max : UP_OBJECT_BUFFER_READ_LENGTH;
    

    int packet_read = up_readNetworkDatabuffer(pipe->simulation_input,objUpdate, max);

    struct up_actionState tmp_states = {0};
    struct up_packet_movement movment = {0};
    //struct up_packet_player_joined player_joind_tmp = {0};
    //struct up_objectInfo obj_tmp = {0};
    
    int i = 0;
    int timestamp = 0;
    int modelId = 0;
    
    int success = 0;
    int index = 0;
    
    int playerId = 0;
    char nameBuffer[256];   // if the user exit we need to pass along the name
    struct up_packet_player_joined playerExit = {0};
    
    for (i = 0; i < packet_read; i++) {
        
        switch (objUpdate[i].data[0]) {
            case  UP_PACKET_ACTION_FLAG:
                up_network_action_packetDecode(&objUpdate[i], &tmp_states, &movment, &timestamp);
                //printf("server pimg ms: %d\n",(SDL_GetTicks() - timestamp));
                success = up_network_updateShipUnit(&tmp_states,&movment);
                index = tmp_states.objectID.idx;
                if (success && (index < UP_MAX_CLIENTS)) {
                    deltaArray[index] = 1;  // tells that there has been a change
                    states[index] = tmp_states;
                }
                break;
            case UP_PACKET_PLAYER_SHIPMODEL_FLAG:
                //up_network_action_packetDecode(&objUpdate[i], &tmp_states, &movment, &timestamp);
                //printf("server pimg ms: %d\n",(SDL_GetTicks() - timestamp));
                //success = up_network_updateShipUnit(&tmp_states,&movment);
                success = up_network_packet_changModelDecode(&objUpdate[i].data[0], &modelId, &index, &timestamp);
        
                if (success && (index < UP_MAX_CLIENTS)) {
                    set_shipModel(modelId,index);
                }
                break;
            case UP_PACKET_PLAYER_EXIT_FLAG:
                success = up_network_packet_playerExit_decode(&objUpdate[i].data[0], nameBuffer, &playerId);
                if (success > 0) {
                    // fill in all player info that should be saved
                    playerExit = getPlayerExitPacket(playerId);
                    // add on the player exit info onto the first packet
                    success += up_intercom_packet_playerJoind_encode(&objUpdate[i].data[success], &playerExit);
                    objUpdate[i].length = success;
                    // send it to the account thread for processing
                    up_writeToNetworkDatabuffer(account->simulation_output, &objUpdate[i]);
                
                }
                up_unit_remove(up_ship_type, playerId);
                states[playerId].objectID.idx = 0;
                
                break;
            default:
                break;
        }
    }
    return max; //check all player slots
    
}

#define UP_GAME_SIM_ACCOUNT_BUFFER 10
int up_game_communication_getAccount(struct up_interThread_communication *accountCom,struct up_interThread_communication *gameCom,struct up_player_stats *statsArray,int max_players)
{
    int max = 5;
    struct objUpdateInformation objUpdate[UP_GAME_SIM_ACCOUNT_BUFFER];
    max = (max < UP_GAME_SIM_ACCOUNT_BUFFER) ? max : UP_GAME_SIM_ACCOUNT_BUFFER;
    
    struct objUpdateInformation newObjdata = {0};
    int packet_read = up_readNetworkDatabuffer(accountCom->simulation_input,objUpdate, max);
    

    struct up_packet_player_joined player_joind_tmp = {0};
    struct up_objectInfo obj_tmp = {0};
    
    int i = 0;
    int timestamp = up_clock_ms();
    
    int success = 0;
    int index = 0;
    
    for (i = 0; i < packet_read; i++) {
        
        switch (objUpdate[i].data[0]) {
            case UP_PACKET_PLAYER_JOINED:
                success = up_intercom_packet_playerJoind_decode(&objUpdate[i].data[0], &player_joind_tmp);
                index = player_joind_tmp.objectID.idx;
                if (success && (index < UP_MAX_CLIENTS)) {
                    obj_tmp = up_asset_createObjFromId(player_joind_tmp.modelId);
                    
                    obj_tmp.objectId = player_joind_tmp.objectID;
                    obj_tmp.modelId = player_joind_tmp.modelId;
                    obj_tmp.pos = player_joind_tmp.pos;
                    // standard start dir
                    obj_tmp.dir.x = 0.03;
                    obj_tmp.dir.y = 1.0;
                    obj_tmp.angle = 0.0;
                    up_server_unit_setObjAtindex(up_ship_type, obj_tmp, index);
                    statsArray[index] = player_joind_tmp.player_stats;
                    up_network_objectmove_packetEncode(&newObjdata, obj_tmp.objectId, obj_tmp.modelId, obj_tmp.pos, obj_tmp.speed, obj_tmp.angle, obj_tmp.bankAngle, timestamp);
                    
                    //send to alla other clients
                    up_writeToNetworkDatabuffer(gameCom->simulation_output, &newObjdata);
                }
                
                break;
            default:
                break;
        }
    }

    return 0;
    
}

// checks all actions parameters, returns true if all equal
static int compare_actions(struct up_actionState *actionA,struct up_actionState *actionB)
{
    return ((actionA->fireWeapon.state == actionB->fireWeapon.state) &&
            (actionA->engine.state == actionB->engine.state) &&
            (actionA->maneuver.state == actionB->maneuver.state));
}

void up_game_communication_sendAction(struct up_actionState *actionArray,int *deltaArray,int numActions,int deltaOn,struct up_interThread_communication *pipe)
{
    struct up_objectInfo *object = NULL;
    
    struct objUpdateInformation updateobject = {0};
    int len  = 0;
    int timestamp = 0;
    int i  =0 ;
    for (i=0; i < numActions; i++) {
        // checks first if the object is active, then if its state have changed
        // we do this to reduce the amount of traffic over the net
        if (actionArray[i].objectID.idx == 0) {
            continue;
        }
        
        
        
        object = up_unit_objAtIndex(actionArray[i].objectID.type, actionArray[i].objectID.idx);

        if (object == NULL) {
            printf("send packet corrupted");
            continue;
        }
        
        if (deltaOn && deltaArray[i] == 0) {
            continue;
        }else
        {
            deltaArray[i] = 0; //set that we have sent it
        }
        

        
        timestamp = up_clock_ms();
        len = up_network_action_packetEncode(&updateobject, &actionArray[i], object->pos, object->speed, object->angle, object->bankAngle,object->modelId, timestamp);
        if (len > 0) {
            updateobject.length = len;
            up_writeToNetworkDatabuffer(pipe->simulation_output, &updateobject);
        }
    }
    
}

static int compare_stats(struct up_player_stats *playerStatsA,struct up_player_stats *playerStatsB)
{
    
    return ((playerStatsA->health.current == playerStatsB->health.current) &&
            (playerStatsA->armor.current == playerStatsB->armor.current) &&
            (playerStatsA->missile.current == playerStatsB->missile.current) &&
            (playerStatsA->bullets.current == playerStatsB->bullets.current) &&
            (playerStatsA->laser.current == playerStatsB->laser.current));
}

void up_game_communication_sendPlayerStats(struct up_player_stats *statsArray,struct up_player_stats *deltaArray,int map_maxPlayers, struct up_interThread_communication *pipe)
{

    struct objUpdateInformation updateobject = {0};
    int len  = 0;
    int timestamp = 0;
    int i  =0 ;
    for (i=0; i < map_maxPlayers; i++) {
        // checks first if the object is active, then if its state have changed
        // we do this to reduce the amount of traffic over the net
        if ((statsArray[i].objectId.idx == 0) ||
            (compare_stats(&statsArray[i],&deltaArray[i]))) {
            continue;
        }
        
        // update delta
        deltaArray[i] = statsArray[i];
        
        timestamp = up_clock_ms();
        len = up_network_playerStats_packetEncode(&updateobject, &statsArray[i], timestamp);
        if (len > 0) {
            updateobject.length = len;
            up_writeToNetworkDatabuffer(pipe->simulation_output, &updateobject);
        }
    }
}

void up_game_communication_sendObjChanged(struct up_objectID *object_movedArray ,int count,struct up_interThread_communication *pipe)
{
    struct up_objectInfo *object = NULL;
    
    struct objUpdateInformation updateobject = {0};
    int len  = 0;
    int timestamp = up_clock_ms();
    int i  =0 ;
    for (i=0; i < count; i++) {
        
        
        object = up_unit_objAtIndex(object_movedArray[i].type, object_movedArray[i].idx);
        if (object == NULL) {
            // object have been removed, send update
            len = up_network_removeObj_packetEncode(&updateobject, object_movedArray[i], timestamp);
            if (len > 0) {
                updateobject.length = len;
                up_writeToNetworkDatabuffer(pipe->simulation_output, &updateobject);
            }
            continue;
        }
        
        // object have changed some way, this encodes and sends the change to all clients,
        len = up_network_objectmove_packetEncode(&updateobject, object->objectId, object->modelId, object->pos, object->speed, object->angle, object->bankAngle, timestamp);
        
        if (len > 0) {
            updateobject.length = len;
            up_writeToNetworkDatabuffer(pipe->simulation_output, &updateobject);
        }
    }
    
}


