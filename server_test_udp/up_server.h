//
//  up_server.h
//  server_test_udp
//
//  Created by o_0 on 2015-05-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_SERVER_H
#define UP_SERVER_H

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include "up_server_game_simulation.h"


#define UP_MAX_CLIENTS 20

#define UP_BUFFER_SIZE 1024
#define UP_SEND_OBJECT_LENGTH 100
#define UP_SEND_BUFFER_DATA_SIZE 400

struct up_interThread_communication
{

    struct up_thread_queue *simulation_input;   // connected to the recive thread
    struct up_thread_queue *simulation_output;  //connected to the send thread
};

struct up_game_simulation_com
{
    int online_signal; // points to a shared shutdown signal
    int mapSeed;
    struct up_interThread_communication *server_gameplay;
    struct up_interThread_communication *server_account;
};

struct up_client_info
{
    struct sockaddr_in client_addr;
    int lastStamp;  // when a client sends data to the server this gets set
    int heartbeat; // every time we recive a msg this gets set to zero
    int active;
    int slot_empty;
};

struct up_server_connection_info
{
    int shutdown;
    int socket_server;
    struct sockaddr_in server_info;
    int connected_clients;
    struct up_client_info client_infoArray[UP_MAX_CLIENTS];
    struct up_interThread_communication *game_com;
    struct up_thread_queue *AccountQueue;
    
};

struct internal_server_state
{
    int isOnline;
    struct up_game_simulation_com *game_simulation;
    pthread_t *game_simulation_thread;
    
    pthread_t *server_gameplay_thread;
    struct up_server_connection_info *server_gameplay;
    pthread_t *server_account_thread;
    struct up_server_connection_info *server_account;
};

int up_server_usersOnline(struct up_server_connection_info *server);
//gives back the idx to be used by the user
int up_server_addUser(struct up_server_connection_info *server,struct sockaddr_in *client_addr);
struct internal_server_state *up_server_startup();
void up_server_run(struct internal_server_state *server_state);
void up_server_shutdown_cleanup(struct internal_server_state *server_state);
#endif /* defined(__server_test_udp__up_server__) */
