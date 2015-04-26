//
//  up_concurrent_test.h
//  concurrentQueue
//
//  Created by o_0 on 2015-04-23.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef concurrentQueue_up_concurrent_test_h
#define concurrentQueue_up_concurrent_test_h

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <SDL2_image/SDL_image.h>
#define SIZE 1000

// this will print the msg, and in what file , function and at what line the error oucured
#define UP_ERROR_MSG(str) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s \n",__FILE__,__func__,__LINE__,str)

#define UP_ERROR_MSG_STR(str1,str2) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s str: %s\n",__FILE__,__func__,__LINE__,str1,str2)
#define UP_ERROR_MSG_INT(str1,var) fprintf(stderr,"\n\nError:\nFile: %s\nFunc: %s\nLine: %d\nmsg: %s var:%d\n",__FILE__,__func__,__LINE__,str1,var)

struct up_vec3
{
    float x,y,z;
};


struct objUpdateInformation
{
    int id;
    int timestamp;
    struct up_vec3 pos;
    struct up_vec3 dir;
    struct up_vec3 speed;
};


int up_concurrentQueue_start_setup();
void up_concurrentQueue_shutdown_deinit();

// this is ment to be used by the udp reciver thread that listens to the socket
// when data arrive from the connection it is passed to up_writeToNetworkDatabuffer
// then when the gameloop need to get its data it uses up_readNetworkDatabuffer

// this is a waitfree lockfree bufferRead
// use this to read the buffer,
int up_readNetworkDatabuffer(struct objUpdateInformation *data,int length);

// use this to store data that comes from the network
int up_writeToNetworkDatabuffer(struct objUpdateInformation *data);

// unittest for the queue
void up_unitTest_concurency_queue_spsc();
#endif
