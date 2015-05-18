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

#define UP_SEND_OBJECT_LENGTH 100
#define UP_SEND_BUFFER_DATA_SIZE 400

struct up_server_connection_info
{
    int shutdown;
    int socket_server;
    struct sockaddr_in server_info;
    int connected_clients;
    struct sockaddr_in client_infoArray[UP_MAX_CLIENTS];
    struct up_thread_queue *queue;
};

struct internal_server_state
{
    pthread_t *server_gameplay_thread;
    struct up_server_connection_info *server_gameplay;
    pthread_t *server_account_thread;
    struct up_server_connection_info *server_account;
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


#define BUFFER_SIZE 1024

//static struct up_server_connection_info *up_server_socket_start();



/******************************************************
 * account comunications
 * this is where the information comes in to the server
 * and where the information get passed to along,
 * and finaly send to all the users
 *******************************************************/

#define UP_REGISTRATE_FLAG (unsigned char)1
#define UP_LOGIN_FLAG (unsigned char)2
#define UP_USER_PASS_FLAG (unsigned char)4

void *up_server_account_reciveing_thread(void *parm)
{

    struct internal_server_state *server_state = (struct internal_server_state *)parm;
    struct up_server_connection_info * server_con = server_state->server_account;
    
    struct sockaddr_in client_sock;
    unsigned int client_sock_len = sizeof(client_sock);

    unsigned char recvBuff[BUFFER_SIZE];
    struct objUpdateInformation local_data = {0};
    
    long msglen = 0;
    
    while (server_con->shutdown == 0) {
        msglen = 0;
        if ((msglen = recvfrom(server_con->socket_server, recvBuff, BUFFER_SIZE, 0, (struct sockaddr *)&client_sock,&client_sock_len))==-1) {
            printf("recvfrom failed\n");
            perror("recfrom failed");
            break;
        }

        if ((msglen > 5) && (msglen < UP_QUEUE_DATA_SIZE)) {
            local_data.length = (int)msglen;
            generic_copyElement((unsigned int)msglen, local_data.data, recvBuff);
            up_writeToNetworkDatabuffer(server_con->queue,&local_data);
            
        }
        
        
        //printf("\nmsg length: %lu",msglen);
    }
    
    printf("\nrecive thread shutdown");
    return NULL;
}

#define UP_USER_NAME_PASS_MAX 21
struct up_account_information
{
    int current_task;
    int account_id;
    char username[UP_USER_NAME_PASS_MAX];
    char password[UP_USER_NAME_PASS_MAX];
};

int up_parser_username_passowrd(struct up_account_information *account_validation,unsigned char *data_parser)
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
    if (data_parser[read_pos] == UP_LOGIN_FLAG) {
        account_validation->current_task = UP_LOGIN_FLAG;
        read_pos++;
        return up_parser_username_passowrd(account_validation,&data_parser[read_pos]);
        
    }
    
    if (data_parser[read_pos] == UP_REGISTRATE_FLAG) {
        account_validation->current_task = UP_REGISTRATE_FLAG;
        read_pos++;
        return up_parser_username_passowrd(account_validation,&data_parser[read_pos]);
        
    }
    
    return 1;
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


/******************************************************
* gamplay comunications
* this is where the information comes in to the server
* and where the information get passed to along, 
* and finaly send to all the users
 *******************************************************/
void *up_server_gamplay_reciveing_thread(void *parm)
{
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    
    struct sockaddr_in client_sock;
    unsigned int client_sock_len = sizeof(client_sock);

    unsigned char recvBuff[BUFFER_SIZE];
    struct objUpdateInformation local_data = {0};
    
    unsigned long msglen = 0;
    int i = 0;
    int userId = 0;
    
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
        
        userId = i;
        
        if (i == server_con->connected_clients ) {
            if (i >=  UP_MAX_CLIENTS) {
                continue;
            }
            server_con->client_infoArray[i] = client_sock;
            userId = i;
            server_con->connected_clients++;
        }
        
        if ((msglen > 5) && (msglen < UP_QUEUE_DATA_SIZE)) {
            local_data.id = userId;
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
    int spin_counter = 0;
    while (packet_read <= 0)
    {
        packet_read = up_readNetworkDatabuffer(server_con->queue,local_data, length);
        spin_counter++;
        usleep(100);
        if (spin_counter > 2000) {
            usleep(1000);
            if (server_con->shutdown) {
                return 0;
            }
            spin_counter = 0;
        }
    }
    return packet_read;
}

static int up_server_send_toAll(struct up_server_connection_info * server_con, struct objUpdateInformation *local_data, int packet_read)
{
    unsigned int client_sock_len = sizeof(server_con->client_infoArray[0]);
    
    int packet_idx = 0;
    int client_idx = 0;
    
    int dataToSend_len = 0;
    for (packet_idx = 0; packet_idx < packet_read; packet_idx++) {
        dataToSend_len = local_data[packet_idx].length;
        for (client_idx = 0; client_idx < server_con->connected_clients; client_idx++) {
            if (sendto(server_con->socket_server, local_data[packet_idx].data, dataToSend_len, 0, (struct sockaddr *)&server_con->client_infoArray[client_idx], client_sock_len) == -1) {
                printf("sendTo error");
                break;
            }
        }
        
    }
    return 0;
}


void *up_server_gameplay_send_thread(void *parm)
{
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    int length = UP_SEND_OBJECT_LENGTH;
    struct objUpdateInformation local_data[UP_SEND_OBJECT_LENGTH];
    
    int packet_read = 0;

    while (server_con->shutdown == 0) {
        
        packet_read = up_server_send_bufferRead_spinloop(local_data, length, server_con);
        
        if (server_con->shutdown) {
            break;
        }
        
        // TODO: decode data and check if correct
        
        up_server_send_toAll(server_con, local_data, packet_read);
        
    }
    
    printf("\nsend thread shutdown");
    return NULL;
}



/******************************************************
 * Server side console interface, used to shutdown server
 * and get information of the thingy
 *******************************************************/

void up_server_run(struct internal_server_state *server_state)
{
    int flag = 1;
    char commands[512];
    while (flag) {
        printf("\nCommands>");
        scanf("%s",commands);
        if (strstr(commands, "exit") != NULL) {
            server_state->server_account->shutdown = 1;
            server_state->server_gameplay->shutdown = 1;
        
            flag = 0;
        }
        
    }
}

/******************************************************
 * Server setup and start sequnce and stuff
 * also shutdown operations
 *******************************************************/

static struct up_server_connection_info *up_server_account_start(unsigned int port)
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
    sock_server.sin_port = htons(port);
    sock_server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(socket_server, (struct sockaddr*)&sock_server, sizeof(struct sockaddr)) == -1) {
        printf("bind failure account server");
        close(socket_server);
        perror("bind error");
        return NULL;
    }
    
    struct up_server_connection_info *server = malloc(sizeof(struct up_server_connection_info));
    if (server == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    server->shutdown = 0;
    server->socket_server = socket_server;
    server->server_info = sock_server;
    return server;
    
    
}


// bind the listening port
static struct up_server_connection_info *up_server_gameplay_start(unsigned int port)
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
    sock_server.sin_port = htons(port);
    sock_server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(socket_server, (struct sockaddr*)&sock_server, sizeof(struct sockaddr)) == -1) {
        printf("bind failure gameplay server");
        close(socket_server);
        perror("bind error");
        return NULL;
    }
    
    struct up_server_connection_info *server = malloc(sizeof(struct up_server_connection_info));
    if (server == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    server->shutdown = 0;
    server->socket_server = socket_server;
    server->server_info = sock_server;
    return server;

    //close(socket_server);
    
}

// starts the threads and recive and store
struct internal_server_state *up_server_startup()
{
    if(!up_concurrentQueue_start_setup())
    {
        UP_ERROR_MSG("queue failed");
    }
    
    
    // gameplay server sockets setup, this starts the sockets at port
    struct up_server_connection_info *server_gameplay = up_server_gameplay_start(22422);
    
    // this starts the fifo buffer between threads (recive and send)
    server_gameplay->queue = up_concurrentQueue_new();
    if (server_gameplay->queue == NULL) {
        UP_ERROR_MSG("failed to start queue");
    }
    
    // here start gameplay server and can now be used for fun
    pthread_t *server_gameplay_thread = malloc(sizeof(pthread_t)*2);
    if (server_gameplay_thread == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    pthread_create(&server_gameplay_thread[0],NULL,&up_server_gamplay_reciveing_thread,server_gameplay);
    pthread_create(&server_gameplay_thread[1],NULL,&up_server_gameplay_send_thread,server_gameplay);
    
    // too keep track of all threads and servers running we need to return
    struct internal_server_state *server_state = malloc(sizeof(struct internal_server_state));
    if (server_state == NULL) {
        UP_ERROR_MSG("failed to malloc server state");
    }
    
    server_state->server_gameplay = server_gameplay;
    server_state->server_gameplay_thread = server_gameplay_thread;
    
    
    // gameplay server sockets setup, this starts the sockets at port
    struct up_server_connection_info *server_account = up_server_account_start(44244);
    
    // this starts the fifo buffer between threads (recive and send)
    server_account->queue = up_concurrentQueue_new();
    if (server_gameplay->queue == NULL) {
        UP_ERROR_MSG("failed to start queue");
    }
    
    // we are going to send the server state to the account part
    server_state->server_account = server_account;
    
    // here start gameplay server and can now be used for fun
    pthread_t *server_account_thread = malloc(sizeof(pthread_t)*2);
    if (server_gameplay_thread == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    pthread_create(&server_account_thread[0],NULL,&up_server_account_reciveing_thread,server_state);
    pthread_create(&server_account_thread[1],NULL,&up_server_account_send_thread,server_state);
    
    
    server_state->server_account_thread = server_account_thread;
    
    return server_state;
}


void up_server_shutdown_cleanup(struct internal_server_state *server_state)
{
    close(server_state->server_gameplay->socket_server); // do this first to force a faile on all connections
    int i = 0;
    for (i = 0; i < 2; i++) {
        pthread_join(server_state->server_gameplay_thread[i], NULL);
    }
    
    
    free(server_state->server_gameplay);
    free(server_state->server_gameplay_thread);
    
    up_concurrentQueue_shutdown_deinit();
    printf("\nserver cleanup done");
    
}
