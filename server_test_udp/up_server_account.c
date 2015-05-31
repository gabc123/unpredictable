//
//  up_server_account.c
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-25.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "up_error.h"
#include "up_server_account.h"
#include "up_thread_utilities.h"
#include "up_network_packet_utilities.h"
#include "up_server.h"
#include "up_utilities.h"

#define SHA256_BLOCK_SIZE 32
/******************************************************
 * account comunications
 * this is where the information comes in to the server
 * and where the information get passed to along,
 * and finaly send to all the users
 *******************************************************/

void *up_server_account_reciveing_thread(void *parm)
{
    
    struct internal_server_state *server_state = (struct internal_server_state *)parm;
    struct up_server_connection_info * server_con = server_state->server_account;
    
    struct sockaddr_in client_sock;
    unsigned int client_sock_len = sizeof(client_sock);
    
    unsigned char recvBuff[UP_BUFFER_SIZE];
    struct objUpdateInformation local_data = {0};
    
    long msglen = 0;
    int i = 0;
    int userId = 0;
    while (server_con->shutdown == 0) {
        msglen = 0;
        if ((msglen = recvfrom(server_con->socket_server, recvBuff, UP_BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,&client_sock_len))==-1) {
            printf("recvfrom failed\n");
            perror("recfrom failed");
            break;
        }
        
        for (i = 1; i < server_con->connected_clients; i++) {
            
            if(server_con->client_infoArray[i].client_addr.sin_addr.s_addr == client_sock.sin_addr.s_addr)
            {
                server_con->client_infoArray[i].heartbeat = 0;  //reset every loop
                // temporary if fix
                if (server_con->client_infoArray[i].active == 0) {
                    server_con->client_infoArray[i].client_addr = client_sock;
                    server_con->client_infoArray[i].lastStamp = 0;
                    server_con->client_infoArray[i].heartbeat = 0;
                    server_con->client_infoArray[i].active = 1;
                    userId = i;
                    printf("\nUser connected: %s",inet_ntoa(client_sock.sin_addr));
                    fflush(stdout);
                }
                server_con->client_infoArray[i].active = 1;
                break;
            }
        }
        
        //userId = i;
        
        if (i == server_con->connected_clients ) {
            if (i >=  UP_MAX_CLIENTS) {
                continue;
            }
            server_con->client_infoArray[i].client_addr = client_sock;
            server_con->client_infoArray[i].lastStamp = 0;
            server_con->client_infoArray[i].heartbeat = 0;
            server_con->client_infoArray[i].active = 1;
            userId = i;
            
            printf("\nUser connected: %s",inet_ntoa(client_sock.sin_addr));
            fflush(stdout);
            server_con->connected_clients++;
        }
        
        
        if ((msglen > 2) && (msglen < UP_QUEUE_DATA_SIZE)) {
            local_data.id = userId;  // if it is 0 then the packet will be skipped
            local_data.length = (int)msglen;
            generic_copyElement((unsigned int)msglen, local_data.data, recvBuff);
            up_writeToNetworkDatabuffer(server_con->AccountQueue,&local_data);
            
        }
        
        
        //printf("\nmsg length: %lu",msglen);
    }
    
    printf("\nrecive thread shutdown");
    return NULL;
}


struct up_account_information
{
    int current_task;
    int account_id;
    char username[UP_USER_NAME_PASS_MAX];
    char password[UP_USER_NAME_PASS_MAX];
};

#define UP_FILEPATH_MAX 200
//Tobias 26-05-2015
int up_logInToAccount(struct up_account_information *account_validation, unsigned char *data_parser)
{
    char userFilePath[UP_FILEPATH_MAX] = "account_information/";
    //char passwordToCompWith[SHA256_BLOCK_SIZE] = {0};
    char passwordCheck;
    int i;
    strcat(userFilePath, (char *)account_validation->username);
    FILE *fp = fopen(userFilePath, "r");
    
    if(fp == NULL)
    {
        fprintf(stderr, "No username found\n");
        return LOGINFAILED;
    }

    for(i=0;i < SHA256_BLOCK_SIZE;i++)
    {
        fscanf(fp, "%c", &passwordCheck);
        if (account_validation->password[i] != passwordCheck) {
            printf("Loginerror\n");
            fflush(stdout);
            fclose(fp);
            return LOGINFAILED;
        }
    }
    
    fclose(fp);
    return LOGINSUCESS;
}

//Tobias 26-05-2015
int up_registrateAccount(struct up_account_information *account_validation, unsigned char *data_parser)
{
    char userFilePath[UP_FILEPATH_MAX] = "account_information/";
    strcat(userFilePath, (char *)account_validation->username);
    FILE *fp = fopen(userFilePath, "r");
    if(fp != NULL) {
        fprintf(stderr, "Username in use");
        fclose(fp);
        return REGFAILED;
    }

    fp = fopen(userFilePath, "w");
    int i = 0;
    for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
        fprintf(fp,"%c",account_validation->password[i]);
    }
    fclose(fp);
    return REGSUCESSS;
}

//Tobias 
int up_parser_username_password(struct up_account_information *account_validation,unsigned char *data_parser)
{
    int user_length = 0;
    int pass_length = 0;
    int read_pos = 0;
    
    if (data_parser[read_pos] != UP_USER_PASS_FLAG) {
        return 0;
    }
    read_pos++;

    
    user_length = data_parser[read_pos];
    printf("\nuser_length: %d",user_length);
    fflush(stdout);
    if (user_length > UP_USER_NAME_PASS_MAX - 1) {
        UP_ERROR_MSG("length is out of bounds");
        return 0;
    }
    read_pos++;

    generic_copyElement(user_length, (unsigned char *)account_validation->username, &data_parser[read_pos]);
    account_validation->username[user_length] = '\0';
    
    read_pos += user_length;
    pass_length = data_parser[read_pos];
    printf("\npass_length: %d",pass_length);
    fflush(stdout);
    if (pass_length > UP_USER_NAME_PASS_MAX - 1) {
        UP_ERROR_MSG("length is out of bounds");
        return 0;
    }
    read_pos++;
    
    generic_copyElement(pass_length, (unsigned char *)account_validation->password, &data_parser[read_pos]);
    account_validation->password[pass_length] = '\0';
    return 1;
}


int up_account_msg_parser(struct up_account_information *account_validation,unsigned char *data_parser)
{
    int read_pos = 0;
    //Checking whether the user wants to registrate och log in to an account
    if (data_parser[read_pos] == UP_LOGIN_FLAG) {
        account_validation->current_task = UP_LOGIN_FLAG;
        read_pos++;
        up_parser_username_password(account_validation,&data_parser[read_pos]);
        return up_logInToAccount(account_validation,&data_parser[read_pos]);
    }
    
    if (data_parser[read_pos] == UP_REGISTRATE_FLAG) {
        account_validation->current_task = UP_REGISTRATE_FLAG;
        read_pos++;
        up_parser_username_password(account_validation,&data_parser[read_pos]);
        return up_registrateAccount(account_validation,&data_parser[read_pos]);

    }
    
    if (data_parser[read_pos] == UP_PACKET_HEARTBEAT_FLAG) {
        
        return UP_PACKET_HEARTBEAT_FLAG;
    }
 
    fprintf(stderr, "Unaccepted first flag on log in/registration process\n");
    fflush(stdout);
    return 0;

}


/*
 struct up_container{
 int current;
 int full;
 };
 
 struct up_player_stats
 {
 int modelId;
 struct up_objectID objectId;
 struct up_container bullets;
 struct up_container missile;
 struct up_container laser;
 struct up_container health;
 struct up_container armor;
 };
 :100/100/50/5/bullet
 :2500/30/30/20/missile
 :100/200/25/5/lazer

 
 */
static void game_simulation_addUser(struct up_interThread_communication *interCom,int playerId,struct up_account_information *account_validation)
{
    char userFilePath[UP_FILEPATH_MAX] = "account_information/";
    strcat(userFilePath, (char *)account_validation->username);
    strcat(userFilePath, "_stats");
    struct up_;
    
    struct up_packet_player_joined player = {0};
    player.modelId = 1;
    player.objectID.idx = playerId;
    player.objectID.type = up_ship_type;
    player.pos.z = 40;
    player.pos.x = (float)(up_rand() % 1000 - 500);
    player.pos.y = (float)(up_rand() % 1000 - 500);
    struct up_player_stats tmp_stats = {1,playerId, up_ship_type, 50,50,30,50,25,50,100,100,100,100};
    
    player.player_stats = tmp_stats;
    struct objUpdateInformation objUpdate = {0};
    unsigned int len;
    FILE *fp = fopen(userFilePath, "r");
    if(fp != NULL) {
        fscanf(fp, "%d ", &player.modelId);
        fscanf(fp, "%f %f %f ", &player.pos.x, &player.pos.y, &player.pos.z);
        fscanf(fp, "%d %d %d %d %d ",&player.player_stats.health.current,
               &player.player_stats.armor.current, &player.player_stats.bullets.current,
               &player.player_stats.missile.current,&player.player_stats.laser.current);
        fclose(fp);
        len = up_intercom_packet_playerJoind_encode(&objUpdate.data[0], &player);
        objUpdate.length = len;
        up_writeToNetworkDatabuffer(interCom->simulation_input, &objUpdate);
        return;
    }
    
    fp = fopen(userFilePath, "w");
    if (fp == NULL) {
        UP_ERROR_MSG("cant create user_pos file");
    }
    fprintf(fp, "%d ", player.modelId);
    fprintf(fp, "%f %f %f ", player.pos.x, player.pos.y, player.pos.z);
    fprintf(fp, "%d %d %d %d %d ",player.player_stats.health.current,
           player.player_stats.armor.current, player.player_stats.bullets.current,
           player.player_stats.missile.current,player.player_stats.laser.current);
    fclose(fp);
    len = up_intercom_packet_playerJoind_encode(&objUpdate.data[0], &player);
    objUpdate.length = len;
    
    up_writeToNetworkDatabuffer(interCom->simulation_input, &objUpdate);
    
}

void *up_server_account_send_thread(void *parm)
{
    struct internal_server_state *server_state = (struct internal_server_state *)parm;
    struct up_server_connection_info * server_con = server_state->server_account;
    
    int length = UP_SEND_OBJECT_LENGTH;
    struct objUpdateInformation local_data[UP_SEND_OBJECT_LENGTH];
    struct up_account_information account_validation;
    int packet_read = 0;
    
    int spin_counter = 0;
    int clientId = 0;
    int returnFlag=0;
    
    
    unsigned char dataBuffer[UP_SEND_BUFFER_DATA_SIZE];
    unsigned int dataToSend_len = 0;
    unsigned int client_sock_len = sizeof(server_con->client_infoArray[0].client_addr);
    struct up_client_info *clientInfo = NULL;
    
    int mapSeed = server_state->game_simulation->mapSeed;
    int usersOnline = 0;
    
    int i = 0;
    while (server_con->shutdown == 0) {
        packet_read =0;
        while (packet_read <= 0)
        {
            
            packet_read = up_readNetworkDatabuffer(server_con->AccountQueue,local_data, length);
            spin_counter++;
            usleep(100);
            if (spin_counter > 2000) {
                usleep(1000);
                if (server_con->shutdown) {
                    break;
                }
                spin_counter = 0;
            }
        }
        
        if (server_con->shutdown) {
            break;
        }
        
        for (i = 0; i < packet_read; i++) {
            
            returnFlag = up_account_msg_parser(&account_validation,local_data[i].data);
            if(returnFlag==0)
            {
                printf("\nAccount Packet corrupted");
                continue;
            }
            
            if(returnFlag==UP_PACKET_HEARTBEAT_FLAG)
            {
                
                continue;
            }
            
            clientInfo = &server_con->client_infoArray[local_data[i].id];
            clientId = 0;
            if (returnFlag == LOGINSUCESS) {
                clientId = up_server_addUser(server_state->server_gameplay, &clientInfo->client_addr);
                usersOnline = up_server_usersOnline(server_state->server_gameplay);
                game_simulation_addUser(server_con->game_com, clientId, &account_validation);
                
            }
            
            
            // the msg should start with if it was a login or registration request
            dataToSend_len = 0;
            dataBuffer[0] = local_data[i].data[0]; // if we gotten here then the flag is correct and can be used again
            dataToSend_len++;
            // encode the rest of the msg
            dataToSend_len += up_network_logInRegistrate_packetEncode(&dataBuffer[dataToSend_len], clientId, returnFlag);
            dataToSend_len += up_network_packet_mapData_encode(&dataBuffer[dataToSend_len], clientId, mapSeed, usersOnline);
            
            if (clientInfo->active != 0) {
                if (sendto(server_con->socket_server, dataBuffer, dataToSend_len, 0, (struct sockaddr *)&clientInfo->client_addr, client_sock_len) == -1) {
                    printf("\nserver sendTo error");
                    perror("send account");
                }
                
                
            }
            
        }
        
    }
    
    printf("\nsend thread shutdown");
    return NULL;
}
