//
//  up_thread_utilities.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-26.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_THREAD_UTILITIES_H
#define UP_THREAD_UTILITIES_H
#include "up_vertex.h"
#include "up_ship.h"

struct __attribute__((__packed__)) up_packed_data
{
    int timestamp;
    struct up_actionState action;
    struct up_vec3 pos;
    float angle;
    float bankAngle;
    float turnSpeed;
    float speed;
};

struct objUpdateInformation
{
    int id;
    struct up_packed_data data; // packed structure to prevent padding when tranforming data into unsigned char to transmitt over the network
};

// forward decleration
struct up_thread_queue;

int up_concurrentQueue_start_setup();
void up_concurrentQueue_shutdown_deinit();

// this is ment to be used by the udp reciver thread that listens to the socket
// when data arrive from the connection it is passed to up_writeToNetworkDatabuffer
// then when the gameloop need to get its data it uses up_readNetworkDatabuffer

// this is a waitfree lockfree bufferRead
// use this to read the buffer,
int up_readNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data,int length);

// use this to store data that comes from the network
int up_writeToNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data);

// unittest for the queue
void up_unitTest_concurency_queue_spsc();

#endif
