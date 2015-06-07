//
//  up_thread_utilities.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-26.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_THREAD_UTILITIES_H
#define UP_THREAD_UTILITIES_H



// all data sent over the thread_queue is containd in this structure
// a id == 0 indicate that the data is already read and will not be used
#define UP_QUEUE_DATA_SIZE 768
struct objUpdateInformation
{
    int id;
    int length;
    unsigned char data[UP_QUEUE_DATA_SIZE];

};

// forward decleration
struct up_thread_queue;

/* before using this module call the setup function*/
int up_concurrentQueue_start_setup();
/* call when all queue have been freed and no more will be created*/
void up_concurrentQueue_shutdown_deinit();

/*
    up_concurrentQueue_new : This starts a new thread_queue and performe all setup 
    returns a pointer to the thread_queue that can be used to communicate between threads
    returns NULL on error
    Needs to be match with a call to up_concurrentQueue_free
 */
struct up_thread_queue *up_concurrentQueue_new();

/*
    Call this to shutdown the thread queue and to free all allocated memory
 */
void up_concurrentQueue_free(struct up_thread_queue *queue);



// this is ment to be used by the udp reciver thread that listens to the socket
// when data arrive from the connection it is passed to up_writeToNetworkDatabuffer
// then when the gameloop need to get its data it uses up_readNetworkDatabuffer

// this is a waitfree lockfree bufferRead
// use this to read the buffer/queue,
int up_readNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data,int length);

// use this to store data into the queue that can then be read with up_readNetworkDatabuffer
int up_writeToNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data);

// unittest for the buffer/queue
void up_unitTest_concurency_queue_spsc();

#endif
