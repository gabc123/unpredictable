//
//  up_server_account.c
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-25.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"
#include "up_server_account.h"
#include "up_thread_utilities.h"
#include "up_network_packet_utilities.h"
#include "up_server.h"
#include <string.h>

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
    
    while (server_con->shutdown == 0) {
        msglen = 0;
        if ((msglen = recvfrom(server_con->socket_server, recvBuff, UP_BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,&client_sock_len))==-1) {
            printf("recvfrom failed\n");
            perror("recfrom failed");
            break;
        }
        
        if ((msglen > 5) && (msglen < UP_QUEUE_DATA_SIZE)) {
            local_data.id = 1;  // if it is 0 then the packet will be skipped
            local_data.length = (int)msglen;
            generic_copyElement((unsigned int)msglen, local_data.data, recvBuff);
            up_writeToNetworkDatabuffer(server_con->queue,&local_data);
            
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

//Tobias 26-05-2015
int up_logInToAccount(struct up_account_information *account_validation, &data_parser[read_pos])
{
    char userFilePath = "account_information/";
    char passwordToCompWith[40] = 0; 
    int i;
    strcat(userFilePath, account_validation->username);
    FILE *fp;
    fopen("%s", userFilePath, "r");
    
    if(fp == NULL)
    {
        printf(stderr, "No username found\n");
        return LOGINFAILED;
    }

    for(i=0;i<39;i++)
    {
        fscanf(fp, "%c", &passwordToCompWith[i]);
    }
    if (strcmp(passwordToCompWith, account_validation->password) != 0)
    {
        printf("Loginerror\n");
        return LOGINFAILED;
    }

    return LOGINSUCESS;
}

//Tobias 26-05-2015
int up_registrateAccount(struct up_account_information *account_validation, &data_parser[read_pos])
{
    char userFilePath = "account_information/"; 
    strcat(userFilePath, account_validation->username);
    FILE *fp;
    fp = fopen("%s", userFilePath, "r");
    if(fp != NULL) {
        printf(stderr, "Username in use");
        return REGFAILED;
    }

    fp = fopen("%s", userFilePath, "w";
    fprintf(fp,"%s\n",userFilePath);
    fclose(fp);
    return REGSUCESS;
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
    
    if (user_length > UP_USER_NAME_PASS_MAX - 1) {
        UP_ERROR_MSG("length is out of bounds");
        return 0;
    }
    read_pos++;

    generic_copyElement(user_length, (unsigned char *)account_validation->username, &data_parser[read_pos]);
    account_validation->username[user_length] = '\0';
    
    read_pos += user_length;
    pass_length = data_parser[read_pos];
    printf("\nuser_length: %d",pass_length);
    
    if (pass_length >= UP_USER_NAME_PASS_MAX - 1) {
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
    
    else if (data_parser[read_pos] == UP_REGISTRATE_FLAG) {
        account_validation->current_task = UP_REGISTRATE_FLAG;
        read_pos++;
        up_parser_username_password(account_validation,&data_parser[read_pos]);
        return up_registrateAccount(account_validation,&data_parser[read_pos]);

    }
 
    printf(stderr, "Unaccepted first flag on log in/registration process\n");
    return 0;

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
    
    
    
    unsigned char dataBuffer[UP_SEND_BUFFER_DATA_SIZE];
    unsigned int dataToSend_len = 0;
    unsigned int client_sock_len = sizeof(server_con->client_infoArray[0]);
    
    
    int i = 0;
    while (server_con->shutdown == 0) {
        packet_read =0;
        while (packet_read <= 0)
        {
            
            packet_read = up_readNetworkDatabuffer(server_con->queue,local_data, length);
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
            
            if(!up_account_msg_parser(&account_validation,local_data[i].data))
            {
                printf("\nPacket corrupted");
                continue;
            }
            
            
        }
        
        dataToSend_len = 0;
        for (i = 0; i < packet_read; i++) {
            dataToSend_len += up_copyObjectIntoBuffer(&local_data[i], &dataBuffer[dataToSend_len]);
        }
        
        for (i = 0; i < server_con->connected_clients; i++) {
            if (sendto(server_con->socket_server, dataBuffer, dataToSend_len, 0, (struct sockaddr *)&server_con->client_infoArray[i], client_sock_len) == -1) {
                printf("sendTo error");
                break;
            }
        }
        
    }
    
    printf("\nsend thread shutdown");
    return NULL;
}
