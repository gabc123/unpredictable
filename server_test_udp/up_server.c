//
//  up_server.c
//  server_test_udp
//
//  Created by o_0 on 2015-05-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_server.h"
#include "up_thread_utilities.h"
#include "up_network_packet_utilities.h"
#include "up_server_account.h"
#include "up_server_gameplay.h"
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


//static struct up_server_connection_info *up_server_socket_start();

int up_server_usersOnline(struct up_server_connection_info *server)
{
    int i = 0;
    int users = 0;
    for (i = 1; i < server->connected_clients; i++) {
        users += (server->client_infoArray[i].active != 0) ? 1 : 0;
    }
    return users;
}

//gives back the idx to be used by the user
int up_server_addUser(struct up_server_connection_info *server,struct sockaddr_in *client_addr)
{
    int i = 0;
    struct up_client_info *client = NULL;
    for (i = 1; i < server->connected_clients; i++) {
        client = &server->client_infoArray[i];
        if (client->slot_empty == 1 && client->active == 0) {
            client->client_addr.sin_addr.s_addr = client_addr->sin_addr.s_addr; // only transfer ip addrees
            client->heartbeat = 0;
            client->slot_empty = 0;
            return i;
        }
    }
    if (server->connected_clients >= UP_MAX_CLIENTS) {
        return 0;
    }
    
    int idx = server->connected_clients;
    
    client = &server->client_infoArray[idx];
    client->client_addr.sin_addr.s_addr = client_addr->sin_addr.s_addr; // only transfer ip addrees
    client->heartbeat = 0;
    client->slot_empty = 0;
    client->active = 0;

    return idx;
}

/******************************************************
 * heartbeat
 * every client has a counter that is reset every time sends msg to server
 * this function incresses that timmer and if it gets to high it sends a heartbeat
 * after the a delay and timmer still to high it removes the client from list
 *******************************************************/
// magnus
static int up_server_hearbeat_send(int socket_server,struct up_client_info *client)
{
    unsigned char buffer[64];
    int length = up_network_heartbeat_packetEncode(buffer, 10);
    unsigned int client_sock_len = sizeof(client->client_addr);
    
    if (sendto(socket_server, buffer, length, 0, (struct sockaddr *)&client->client_addr, client_sock_len) == -1) {
        printf("heartbeat sendTo error");
        fflush(stdout);
        return 0;
    }
    return 1;
}
// magnus
static int up_server_remove_client(struct up_client_info *client)
{
    //remove client
    printf("\nUser disconnected: %s",inet_ntoa(client->client_addr.sin_addr));
    fflush(stdout);
    // problematic, if server started to send, and account thread see active = 0
    // and account writes partally over client_addr , we may have a problem
    // the result will be that the server_send gets error , but we can liv with that
    // hopfully...
    client->active = 0;
    client->heartbeat = 0;
    client->slot_empty = 1;
    return 1;
}

// magnus
void *up_server_heartbeat(void *parm)
{
    printf("Gameplay server send thread online");
    struct up_server_connection_info * server_con = (struct up_server_connection_info *)parm;
    int i = 0;
    struct up_client_info *client = NULL;
    while (server_con->shutdown == 0) {
        usleep(1000);
        
        // loops over all clients and incresses the heartbeat counter
        // check if we need to send a heartbeat to the client and sends it if needed,
        // if heartbeat counter gets too high then the client gets droped
        for (i = 0; i < server_con->connected_clients; i++) {
            client = &server_con->client_infoArray[i];
            if(client->active != 1)
            {
                continue;
            }
            
            // add then check if heartbeat need to be send or client dropped
            client->heartbeat++;
            if (client->heartbeat == 1000) {   // one secound
                up_server_hearbeat_send(server_con->socket_server, client);
            }
            
            if (client->heartbeat > 5000) {    // 5 secounds -> disconect
                up_server_remove_client(client);
                
            }
            
        }
        
        
    }
    
    return NULL;
}



/******************************************************
 * Server side console interface, used to shutdown server
 * and get information of the thingy
 *******************************************************/
// magnus
static int up_server_display_users(struct up_server_connection_info *server_info, const char *name)
{
    struct up_client_info *client = NULL;
    char strAddress[INET_ADDRSTRLEN];
    int num_active = 0;
    printf("\n*********************\n");
    printf("Users conneced on %s socket:\n",name);
    printf("idx\tip\tport\theartbeet\n");
    
    int i = 0;
    for (i = 0; i < server_info->connected_clients; i++) {
        client = &server_info->client_infoArray[i];
        if (client->active != 0) {
            num_active++;
            inet_ntop(AF_INET, &(client->client_addr.sin_addr), strAddress, INET_ADDRSTRLEN);
            printf("%d\t%s\t%d\t%d\n",i ,strAddress,ntohs(client->client_addr.sin_port),client->heartbeat);
        }
    }
    printf("Active: %d\n",num_active);
    return num_active;
}

// magnus
void up_server_command_show_online(struct internal_server_state *server_state)
{
    //fflush(stdout);// so we dont mix diffrent threads output
    struct up_server_connection_info *server_account = NULL;
    struct up_server_connection_info *server_gameplay = NULL;
    int numOnline = 0;
    
    server_account = server_state->server_account;
    numOnline += up_server_display_users(server_account,"Account");
    
    server_gameplay = server_state->server_gameplay;
    numOnline += up_server_display_users(server_gameplay, "GamePlay");
    printf("Totol users online: %d\n\n",numOnline);
    fflush(stdout);
}


/*
 server console function, from this function the user can trun off the server
 or show what usersr are online
 
 */
//// magnus
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
            server_state->game_simulation->online_signal = 0;
        
            flag = 0;
            continue;
        }
        if (strstr(commands, "show_online") != NULL) {
            up_server_command_show_online(server_state);
        }

        
    }
}

/******************************************************
 * Server setup and start sequnce and stuff
 * also shutdown operations
 *******************************************************/
// magnus
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
    
    int i = 0;
    for (i = 0; i < UP_MAX_CLIENTS; i++) {
        server->client_infoArray[i].active = 0;
        server->client_infoArray[i].heartbeat = 0;
    }
    
    server->connected_clients = 2;
    server->shutdown = 0;
    server->socket_server = socket_server;
    server->server_info = sock_server;
    return server;
    
    
}


// bind the listening port
// magnus
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
    
    int i = 0;
    for (i = 0; i < UP_MAX_CLIENTS; i++) {
        server->client_infoArray[i].active = 0;
        server->client_infoArray[i].heartbeat = 0;
    }
    
    server->connected_clients = 2;
    server->shutdown = 0;
    server->socket_server = socket_server;
    server->server_info = sock_server;
    
    return server;

    //close(socket_server);
    
}

// starts the threads and recive and store
#define UP_SERVER_GAMEPLAY_THREAD_COUNT 3
#define UP_SERVER_ACCOUNT_THREAD_COUNT 3


/*
    creates, 2 way ,  communications, by using 2 queues, and packated them in a structure
 */
// magnus
static struct up_interThread_communication *up_interThread_com_create()
{
    struct up_interThread_communication *interCom =  malloc(sizeof(struct up_interThread_communication));
    if (interCom == NULL) {
        UP_ERROR_MSG("failed to malloc server state");
        return NULL;
    }
    
    interCom->simulation_input = up_concurrentQueue_new();
    if (interCom->simulation_input == NULL) {
        UP_ERROR_MSG("failed to start queue");
    }
    
    interCom->simulation_output = up_concurrentQueue_new();
    if (interCom->simulation_output == NULL) {
        UP_ERROR_MSG("failed to start queue");
    }
    return interCom;
}

// frees a interThread com free,
// magnus
static void up_interThread_com_free(struct up_interThread_communication *intercom)
{
    up_concurrentQueue_free(intercom->simulation_input);
    up_concurrentQueue_free(intercom->simulation_output);
    intercom->simulation_input = NULL;
    intercom->simulation_output = NULL;
    free(intercom);
}

/*
    starts all server threads. and configure all socets, 
    this is the main starting point for the program, called from main
    it returns at structure contatin all things needed to keep track of the server
 */
// magnus
struct internal_server_state *up_server_startup()
{
    if(!up_concurrentQueue_start_setup())
    {
        UP_ERROR_MSG("queue failed");
    }
    
    // too keep track of all threads and servers running we need to return
    struct internal_server_state *server_state = malloc(sizeof(struct internal_server_state));
    if (server_state == NULL) {
        UP_ERROR_MSG("failed to malloc server state");
    }
    
    
    struct up_game_simulation_com *game_simulation = malloc(sizeof(struct up_game_simulation_com));
    if (server_state == NULL) {
        UP_ERROR_MSG("failed to malloc server state");
    }
    
    server_state->game_simulation = game_simulation;
    
    // two way comunication between game simulation and gameplay server
    struct up_interThread_communication *game_interCom =  up_interThread_com_create();
    if (game_interCom == NULL) {
        UP_ERROR_MSG("failed to start game inter com");
    }
    
    
    server_state->game_simulation->server_gameplay = game_interCom;
    
    // to way comunication to account send
    struct up_interThread_communication *account_interCom =  up_interThread_com_create();
    if (account_interCom == NULL) {
        UP_ERROR_MSG("failed to start game inter com");
    }
    server_state->game_simulation->server_account = account_interCom;
    
    game_simulation->online_signal = 1;
    
    // gameplay server sockets setup, this starts the sockets at port
    struct up_server_connection_info *server_gameplay = up_server_gameplay_start(22422);
    server_gameplay->AccountQueue = NULL;
    // this starts the fifo buffer between threads (recive and send)
    server_gameplay->game_com = game_interCom;

    // here start gameplay server and can now be used for fun
    pthread_t *server_gameplay_thread = malloc(sizeof(pthread_t)*UP_SERVER_GAMEPLAY_THREAD_COUNT);
    if (server_gameplay_thread == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    pthread_create(&server_gameplay_thread[0],NULL,&up_server_gamplay_reciveing_thread,server_gameplay);    //recv thread
    pthread_create(&server_gameplay_thread[1],NULL,&up_server_gameplay_send_thread,server_gameplay);        //send thread
    pthread_create(&server_gameplay_thread[2],NULL,&up_server_heartbeat,server_gameplay);                   // heartbeat
    
    
    
    server_state->server_gameplay = server_gameplay;
    server_state->server_gameplay_thread = server_gameplay_thread;
    server_state->game_simulation->mapSeed = 42; // this is the map all players load in the begining of a game
    
    // gameplay server sockets setup, this starts the sockets at port
    struct up_server_connection_info *server_account = up_server_account_start(44244);
    server_account->game_com = account_interCom;
    // this starts the fifo buffer between threads (recive and send)
    server_account->AccountQueue = up_concurrentQueue_new();
    if (server_account->AccountQueue == NULL) {
        UP_ERROR_MSG("failed to start queue");
    }
    
    // we are going to send the server state to the account part
    server_state->server_account = server_account;
    
    // here start gameplay server and can now be used for fun
    pthread_t *server_account_thread = malloc(sizeof(pthread_t)*UP_SERVER_ACCOUNT_THREAD_COUNT);
    if (server_gameplay_thread == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    pthread_create(&server_account_thread[0],NULL,&up_server_account_reciveing_thread,server_state);
    pthread_create(&server_account_thread[1],NULL,&up_server_account_send_thread,server_state);
    pthread_create(&server_account_thread[2],NULL,&up_server_heartbeat,server_state->server_account);
    
    server_state->server_account_thread = server_account_thread;
    
    
    pthread_t *server_game_simulation_thread = malloc(sizeof(pthread_t));
    if (server_game_simulation_thread == NULL) {
        UP_ERROR_MSG("malloc failure");
    }
    pthread_create(server_game_simulation_thread,NULL,&up_game_simulation,game_simulation);
    
    server_state->game_simulation_thread = server_game_simulation_thread;
    
    
    return server_state;
}

/*
    frees all memory and close all sockets used by the server
    waits on all thread to finnish , and shut down all thread queue
 */
// magnus
void up_server_shutdown_cleanup(struct internal_server_state *server_state)
{
    
    // ugly hack, to force recvfrom to not block forever and exit on some machine
    // we need to send a msg to it, this is solved with our terminator socket that sends to loopback on port 22422 and 44244
    char *dataBuffer = "bye";   //INADDR_LOOPBACK
    struct sockaddr_in serverAddr;
    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("\nstart terminator socket\n");
        perror("terminator socket error\n");
    }
    
    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(22422);
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    //127.0.0.1

    
    if (sendto(sock, dataBuffer, strlen(dataBuffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("\nserver kll msg gameplay\n");
        perror("send gameplay");
    }

    serverAddr.sin_port = htons(44244);
    if (sendto(sock, dataBuffer, strlen(dataBuffer), 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) == -1) {
        printf("\nserver kll msg account\n");
        perror("send account");
    }

    close(sock);
    close(server_state->server_account->socket_server);
    close(server_state->server_gameplay->socket_server); // do this first to force a faile on all connections that are left
    
    int i = 0;
    for (i = 0; i < UP_SERVER_ACCOUNT_THREAD_COUNT; i++) {
        pthread_join(server_state->server_account_thread[i], NULL);
    }
    
    for (i = 0; i < UP_SERVER_GAMEPLAY_THREAD_COUNT; i++) {
        pthread_join(server_state->server_gameplay_thread[i], NULL);
    }
    
    pthread_join(*server_state->game_simulation_thread, NULL);
    
    
    struct up_game_simulation_com *game_sim = server_state->game_simulation;
    server_state->game_simulation = NULL;
    
    struct up_interThread_communication *interCom = game_sim->server_account;
    game_sim->server_account = NULL;
    up_interThread_com_free(interCom);
    
    interCom = game_sim->server_gameplay;
    game_sim->server_gameplay = NULL;
    up_interThread_com_free(interCom);
    
    free(game_sim);
    
    up_concurrentQueue_free(server_state->server_account->AccountQueue);
    
    
    free(server_state->server_gameplay);
    free(server_state->server_gameplay_thread);
    free(server_state->server_account);
    free(server_state->server_account_thread);
    
    free(server_state->game_simulation_thread);
    
    free(server_state);
    
    up_concurrentQueue_shutdown_deinit();
    printf("\nserver cleanup done");
    
}
