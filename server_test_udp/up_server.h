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

#define UP_MAX_CLIENTS 20

#define UP_BUFFER_SIZE 1024
#define UP_SEND_OBJECT_LENGTH 100
#define UP_SEND_BUFFER_DATA_SIZE 400


struct up_client_info
{
    struct sockaddr_in client_addr;
    int lastStamp;  // when a client sends data to the server this gets set
    int heartbeat; // every time we recive a msg this gets set to zero
    int active;
};

struct up_server_connection_info
{
    int shutdown;
    int socket_server;
    struct sockaddr_in server_info;
    int connected_clients;
    struct up_client_info client_infoArray[UP_MAX_CLIENTS];
    struct up_thread_queue *queue;
};

struct internal_server_state
{
    pthread_t *server_gameplay_thread;
    struct up_server_connection_info *server_gameplay;
    pthread_t *server_account_thread;
    struct up_server_connection_info *server_account;
};

struct internal_server_state *up_server_startup();
void up_server_run(struct internal_server_state *server_state);
void up_server_shutdown_cleanup(struct internal_server_state *server_state);
#endif /* defined(__server_test_udp__up_server__) */
