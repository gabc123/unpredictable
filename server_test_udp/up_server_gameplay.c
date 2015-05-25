//
//  up_server_gameplay.c
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-25.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_server_gameplay.h"
#include <stdio.h>
#include <stdlib.h>
#include "up_error.h"
#include "up_thread_utilities.h"
#include "up_network_packet_utilities.h"
#include "up_server.h"


/******************************************************
 * gamplay comunications
 * this is where the information comes in to the server
 * and where the information get passed to along,
 * and finaly send to all the users
 *******************************************************/
void *up_server_gamplay_reciveing_thread(void *parm)
{
    printf("Gameplay server recive thread online");
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    
    struct sockaddr_in client_sock;
    unsigned int client_sock_len = sizeof(client_sock);
    
    unsigned char recvBuff[UP_BUFFER_SIZE];
    struct objUpdateInformation local_data = {0};
    
    unsigned long msglen = 0;
    int i = 0;
    int userId = 0;
    
    while (server_con->shutdown == 0) {
        msglen = 0;
        if ((msglen = recvfrom(server_con->socket_server, recvBuff, UP_BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,&client_sock_len))==-1) {
            printf("recvfrom failed\n");
            perror("recfrom failed");
            break;
        }
        
        for (i = 0; i < server_con->connected_clients; i++) {
            
            if(server_con->client_infoArray[i].client_addr.sin_addr.s_addr == client_sock.sin_addr.s_addr)
            {
                server_con->client_infoArray[i].heartbeat = 0;  //reset every loop
                // temporary if fix
                if (server_con->client_infoArray[i].active == 0) {
                    server_con->client_infoArray[i].client_addr = client_sock;
                    server_con->client_infoArray[i].lastStamp = 0;
                    server_con->client_infoArray[i].heartbeat = 0;
                    server_con->client_infoArray[i].active = 1;
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
            
            printf("\nUser connected: %s",inet_ntoa(client_sock.sin_addr));
            fflush(stdout);
            userId = i + 1; // must be greater then 0
            server_con->connected_clients++;
        }
        
        if ((msglen > 5) && (msglen < UP_QUEUE_DATA_SIZE + 1)) {
            local_data.id = 1;
            local_data.length = (int)msglen;
            generic_copyElement((unsigned int)msglen, local_data.data, recvBuff);
            up_writeToNetworkDatabuffer(server_con->queue,&local_data);
            
        }
        
        //printf("\nmsg length: %lu",msglen);
    }
    //close(socket_server);
    //close(socket_server);
    
    printf("\nrecive thread shutdown");
    return NULL;
}


static int up_server_send_bufferRead_spinloop(struct objUpdateInformation *local_data,int length,struct up_server_connection_info * server_con)
{
    int packet_read =0;
    int spin_counter = 0;   // this is used to prevent the cpu from spining at 100 %cpu
    int breakout_counter = 0;   // we exit the function after a while
    while (packet_read <= 0)
    {
        spin_counter++;
        usleep(100);
        if (spin_counter > 2000) {
            if (server_con->shutdown) {
                return 0;
            }
            
            breakout_counter++;
            if (breakout_counter > 10) {
                return 0;
            }
            
            spin_counter = 0;
            usleep(1000);
        }
        
        packet_read = up_readNetworkDatabuffer(server_con->queue,local_data, length);
    }
    return packet_read;
}

static int up_server_send_toAll(struct up_server_connection_info * server_con, struct objUpdateInformation *local_data, int packet_read)
{
    unsigned int client_sock_len = sizeof(server_con->client_infoArray[0].client_addr);
    
    int packet_idx = 0;
    int client_idx = 0;
    struct up_client_info *clientInfo = NULL;
    
    unsigned char *data = NULL;
    int data_len = 0;
    for (packet_idx = 0; packet_idx < packet_read; packet_idx++) {
        data_len = local_data[packet_idx].length;
        data = local_data[packet_idx].data;
        if (data_len < 2 || data == NULL) {
            continue;
        }
        for (client_idx = 0; client_idx < server_con->connected_clients; client_idx++) {
            
            clientInfo = &server_con->client_infoArray[client_idx];
            if (clientInfo->active != 0) {
                if (sendto(server_con->socket_server, data, (unsigned int)data_len, 0, (struct sockaddr *)&clientInfo->client_addr, client_sock_len) == -1) {
                    printf("\nserver sendTo error");
                    perror("send");
                }
                /*if (sendto(server_con->socket_server, local_data[packet_idx].data, local_data[packet_idx].length, 0, (struct sockaddr *)&clientInfo->client_addr, client_sock_len) == -1) {
                 printf("\nserver sendTo error");
                 perror("send");
                 break;
                 }*/
                
            }
            
        }
        
    }
    return 0;
}


void *up_server_gameplay_send_thread(void *parm)
{
    printf("Gameplay server send thread online");
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    int length = UP_SEND_OBJECT_LENGTH;
    struct objUpdateInformation local_data[UP_SEND_OBJECT_LENGTH];
    
    int packet_read = 0;
    
    while (server_con->shutdown == 0) {
        
        packet_read = up_server_send_bufferRead_spinloop(&local_data[0], length, server_con);
        
        if (server_con->shutdown) {
            break;
        }
        
        // TODO: decode data and check if correct
        if (packet_read > 0) {
            up_server_send_toAll(server_con, &local_data[0], packet_read);
        }
        
        
    }
    
    printf("\nsend thread shutdown");
    return NULL;
}