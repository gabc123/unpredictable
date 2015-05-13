//
//  up_server.c
//  server_test_udp
//
//  Created by o_0 on 2015-05-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_server.h"
#include "up_thread_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include "up_error.h"

#define UP_MAX_CLIENTS 20

#define UP_SEND_OBJECT_LENGTH 4
#define UP_SEND_BUFFER_DATA_SIZE 400

struct up_server_connection_info
{
    int shutdown;
    int socket_server;
    struct sockaddr_in server_info;
    int connected_clients;
    struct sockaddr_in client_infoArray[UP_MAX_CLIENTS];
    
};

struct internal_server_state
{
    pthread_t *server_thread;
    struct up_server_connection_info *server;
};


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
    generic_copyElement(data_len,buffer,(unsigned char *)&object->data);
    return data_len;
}

unsigned int  up_copyBufferIntoObject(unsigned char *buffer,struct objUpdateInformation *object)
{
    unsigned int data_len = sizeof(object->data);
    generic_copyElement(data_len,(unsigned char *)&object->data,buffer);
    return data_len;
}




static struct up_server_connection_info *up_server_socket_start();




void *up_server_reciveing_thread(void *parm)
{
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    
    struct sockaddr_in client_sock;
    unsigned int client_sock_len = sizeof(client_sock);
#define BUFFER_SIZE 1024
    unsigned char recvBuff[BUFFER_SIZE];
    struct objUpdateInformation local_data = {0};
    
    unsigned long msglen = 0;
    int i = 0;
    while (server_con->shutdown == 0) {
        msglen = 0;
        if ((msglen = recvfrom(server_con->socket_server, recvBuff, BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,&client_sock_len))==-1) {
            printf("recvfrom failed\n");
            perror("recfrom failed");
            break;
        }
        
        for (i = 0; i < server_con->connected_clients; i++) {
            if(server_con->client_infoArray[i].sin_addr.s_addr == client_sock.sin_addr.s_addr)
            {
                break;
            }
        }
        
        if (i == server_con->connected_clients ) {
            if (i >=  UP_MAX_CLIENTS) {
                continue;
            }
            server_con->client_infoArray[i] = client_sock;
            server_con->connected_clients++;
        }
        if (msglen < sizeof(local_data.data)) {
            printf("\ntrash msg %lu",msglen);
            continue;
        }
        printf("\npacket recived with length: %lu",msglen);
        up_copyBufferIntoObject(recvBuff,&local_data);
        
        up_writeToNetworkDatabuffer(&local_data);
        
        //printf("\nmsg length: %lu",msglen);
    }
    //close(socket_server);
    //close(socket_server);

    printf("\nrecive thread shutdown");
    return NULL;
}






void *up_server_send_thread(void *parm)
{
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    int length = UP_SEND_OBJECT_LENGTH;
    struct objUpdateInformation local_data[UP_SEND_OBJECT_LENGTH];
    
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
            
            packet_read = up_readNetworkDatabuffer(local_data, length);
            spin_counter++;
            usleep(100);
            if (spin_counter > 2000) {
                usleep(1000);
                if (server_con->shutdown) {
                    break;
                }
            }
        }
        
        if (server_con->shutdown) {
            break;
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



struct internal_server_state *up_server_startup()
{
    if(!up_concurrentQueue_start_setup())
    {
        UP_ERROR_MSG("queue failed");
    }
    struct up_server_connection_info *server = up_server_socket_start();
    pthread_t *server_thread = malloc(sizeof(pthread_t)*2);
    
    pthread_create(&server_thread[0],NULL,&up_server_reciveing_thread,server);
    pthread_create(&server_thread[1],NULL,&up_server_send_thread,server);
    
    struct internal_server_state *server_state = malloc(sizeof(struct internal_server_state));
    if (server_state == NULL) {
        UP_ERROR_MSG("failed to malloc server state");
    }

    server_state->server = server;
    server_state->server_thread = server_thread;
    
    return server_state;
}

void up_server_run(struct internal_server_state *server_state)
{
    int flag = 1;
    char commands[512];
    while (flag) {
        printf("\nCommands>");
        scanf("%s",commands);
        if (strstr(commands, "exit") != NULL) {
            server_state->server->shutdown = 1;
            flag = 0;
        }
        
    }
}

void up_server_shutdown_cleanup(struct internal_server_state *server_state)
{
    close(server_state->server->socket_server); // do this first to force a faile on all connections
    int i = 0;
    for (i = 0; i < 2; i++) {
        pthread_join(server_state->server_thread[i], NULL);
    }
    
    free(server_state->server);
    free(server_state->server_thread);
    up_concurrentQueue_shutdown_deinit();
    printf("\nserver cleanup done");
    
}

static struct up_server_connection_info *up_server_socket_start()
{
    struct sockaddr_in sock_server = {0};
    
    
    int socket_server;
    //unsigned int sock_client_len = sizeof(sock_client);
    
    if((socket_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("err, failed scoket\n");
        return NULL;
    }
    
    memset((char *)&sock_server, 0, sizeof(sock_server));
    
    sock_server.sin_family = AF_INET;
    sock_server.sin_port = htons(22422);
    sock_server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(socket_server, (struct sockaddr*)&sock_server, sizeof(struct sockaddr)) == -1) {
        printf("bind failure");
        close(socket_server);
        perror("bind error");
        return NULL;
    }
    
    struct up_server_connection_info *server = malloc(sizeof(struct up_server_connection_info));
    server->socket_server = socket_server;
    server->server_info = sock_server;
    return server;

    //close(socket_server);
    
}