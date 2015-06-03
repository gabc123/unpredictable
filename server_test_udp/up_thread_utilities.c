//
//  up_thread_utilities.c
//  testprojectshader
//
//  Created by o_0 on 2015-04-26.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_thread_utilities.h"

//#include "stdatomic.h"
#include "up_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //we need usleep



#define UP_BUFFER_1 0
#define UP_BUFFER_2 1


struct up_linkElement
{
    struct up_linkElement *next;
    //atomic_intptr_t next;
    struct objUpdateInformation obj;
};

// dubble buffer, where the buffer is made of a linklist for dynamic size
// its lockfree and theadsafe
// created by magnus brain
struct up_concurrentQueue
{
    //SDL_atomic_t writer_head;
    int writer_head;
    int reader;
    // buffer 1
    struct up_linkElement *first[2];
    struct up_linkElement *last[2];
    
};

struct up_concurrentQueue_gabageRecycler
{
    struct up_linkElement *start;
    struct up_linkElement *end;
};

struct up_thread_queue
{
    struct up_concurrentQueue *live_chain;
    struct up_concurrentQueue_gabageRecycler garbage;
};

//static struct up_concurrentQueue *internal_concurrentQueue = NULL;


static struct up_linkElement *linkElement_alloc();
static struct up_linkElement *linkElement_create(struct up_thread_queue *queue);

static void linkElement_recycle(struct up_thread_queue *queue,struct up_linkElement * fromLink,struct up_linkElement * toLink);

// lockfree waitfree reader on a buffer
int up_readNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data,int length)
{
    // move the writer to the other buffer
    int reader = queue->live_chain->reader;  //if error then setup has not been called early enugh
    struct up_linkElement *first = NULL;
    struct up_linkElement *last = NULL;
    struct up_linkElement *tmpLink = NULL;
    
    //reader = queue->live_chain.reader;
    // change the writerhead atomicly so the writer will work on the other
    // linklist buffer,(it do not matter if the last node change between operations,
    // the writer look for the end always, it only matter that it is set atmoicly )
    
    //SDL_AtomicSet(&queue->live_chain.writer_head, reader);
    queue->live_chain->writer_head = reader;
    
    // change the reader variable so it matches current state
    reader = (reader == UP_BUFFER_1) ? UP_BUFFER_2 : UP_BUFFER_1;
    
    first = queue->live_chain->first[reader];
    last = queue->live_chain->last[reader];
    
    // it is empty
    if (first == NULL || first == last ) {
        queue->live_chain->reader = reader;  //change the reader position so we can get data on other que
        return 0;
    }
    
    int count = 0;
    struct up_linkElement *current = first;
    
    
    // check if the first obj is a dummy link,
    // if so move passed it
    if (current->obj.id == 0) {
        current = current->next;
    }
    
    if (current == NULL) {
        //weired, somehow last came before first
        return 0;
    }
    // if current == last, then the while loop never gets executed
    // so this make sure this is not null at the end of the function.
    tmpLink = first;
    
    int i = 0;
    // walk the linklist until the last link
    // notice that the last element never gets reaped
    // this is becouse we do not want to have a empty queue,
    // some one needs to be first/last to be able to connect to
    while ((current->next != NULL) && (count < length) ) {
        if (current->obj.id == 0) {
            current = current->next;
            continue;
        }
        tmpLink = current;
        data[count].id = tmpLink->obj.id;
        data[count].length = tmpLink->obj.length;
        for (i = 0; i < tmpLink->obj.length; i++) {
             data[count].data[i] = tmpLink->obj.data[i];
        }
        //data[count] = tmpLink->obj;
        count++;
        current = tmpLink->next;
        
    }
    
    // we want to read the last link also but not remove it, if there is room
    if ((current->next == NULL) && (count < length) && (current->obj.id != 0)) {
        
        data[count].id = current->obj.id;
        data[count].length = current->obj.length;
        for (i = 0; i < current->obj.length; i++) {
            data[count].data[i] = current->obj.data[i];
        }
        current->obj.id = 0;
        count++;
        
    }
    
    // move the first pointer to the correct location for continued reading
    queue->live_chain->first[reader] = current;
    
    // this repurpouse the link chain to be used again
    // reducing memory overhead, and waste
    linkElement_recycle(queue,first,tmpLink);
    
    return count; //succsess
}

int up_writeToNetworkDatabuffer(struct up_thread_queue *queue,struct objUpdateInformation *data)
{
    struct up_linkElement *newData = linkElement_create(queue);
    if (newData == NULL) {
        return 0;
    }
    
    // if msg is short we do not need to copy the whole buffer
    int i = 0;
    // 0 == no data
    newData->obj.id = (data->id != 0) ? data->id : 1;
    
    newData->obj.length = data->length;
    for (i = 0; i < data->length; i++) {
        newData->obj.data[i] = data->data[i];
    }
    
    newData->next = NULL;
    struct up_linkElement *currentLast = NULL;
    int whatBuffer = -1;
    int check = 0;
    int counter_failed = 0;
    do {
        /*if (whatBuffer != (check = SDL_AtomicGet(&queue->live_chain.writer_head))) {
         currentLast = queue->live_chain.last[check];
         whatBuffer = check;
         }else
         {
         currentLast = currentLast->next;
         }*/
        if (whatBuffer != ((check = queue->live_chain->writer_head))) {
            currentLast = queue->live_chain->last[check];
            whatBuffer = check;
        }else
        {
            currentLast = currentLast->next;
            counter_failed++;
            if (counter_failed > 5000) {
                printf("counter failed: %d",counter_failed);
                counter_failed = 0;
            }
            
        }
        //https://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html compiler bulltins
    }while (__sync_bool_compare_and_swap(&currentLast->next, NULL, newData) == 0);
    
    queue->live_chain->last[whatBuffer] = newData;
    return 1;
}


//    SDL_AtomicSetPtr(<#void **a#>, <#void *v#>)
static struct up_linkElement *linkElement_alloc()
{
    struct up_linkElement *link = malloc(sizeof(struct up_linkElement));
    if (link == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    link->next = NULL;
    link->obj.id = 0;
    return link;
}





// consume the recyled list if there is anyone
// else it will allocate a new link
static struct up_linkElement *linkElement_create(struct up_thread_queue *queue)
{
    int loopCounter = 0;
    struct up_linkElement *link = NULL;
    do {
        loopCounter++;
        if (loopCounter >= 1000) {
            usleep(10); //sleep to reduce contension on the queue->garbage
        }
        link = queue->garbage.start;
        if (link == NULL) {
            return linkElement_alloc();
        }
        
        
        //we do not want to return the last element
        if (queue->garbage.start == queue->garbage.end)
        {
            return linkElement_alloc();
        }
        
        
        //when a link gets recycle there is a chance that the end link has not been updated
        // in that case we check to se if last , if so alloc a new linkelement
        if (link->next == NULL) {
            return linkElement_alloc();
        }
        
        // now we can relink the list and return our recycled link
        // makes sure we have not gotten a failed link
    } while (__sync_bool_compare_and_swap(&queue->garbage.start, link, link->next) == 0);
    
    // now we can relink the list and return our recycled link
    queue->garbage.start = link->next;
    link->next = NULL; //decouple link from this
    return link;
}

//#define UP_PRODUCTION
// we have a linklist of unused links,
// when a link is not needed any more the next element
// cpu reordering information: http://www.rdrop.com/users/paulmck/scalability/paper/ordering.2007.09.19a.pdf
//
static void linkElement_recycle(struct up_thread_queue *queue,struct up_linkElement * fromLink,struct up_linkElement * toLink)
{
#ifndef UP_PRODUCTION
    if (fromLink == NULL) {
        UP_ERROR_MSG("fromLink == NULL");
    }
    if (toLink == NULL) {
        UP_ERROR_MSG("toLink == NULL");
    }
#endif
    // Too prevent a situvation where the compiler or cpu reorders the following instructions,
    // we need to set the next ptr to null atomicly (prevents the cpu from reordering(x86))
    // and then a memorybarrier to prevent the compiler from reordering stores,
    // if we do not a 1 in a billion type situvation can happen,
    // where the recycler leaks into the active buffer and boom .
    // by using a atomic set and then do a memory barrier we force sequential consistency,
    // on both compiler reordering and cpu reordering (hoppfully)
    //struct up_linkElement *next = toLink->next;
    __sync_lock_test_and_set(&toLink->next, NULL);  //decouple this link chain from the buffer
    
    //SDL_MemoryBarrierRelease();
    
    // connect the chain to the recycler for use
    queue->garbage.end->next = fromLink; // !! must happen after tolink->next = NULL
    
    queue->garbage.end = toLink;
}

struct up_thread_queue *up_concurrentQueue_new()
{
    struct up_thread_queue *thread_queue = malloc(sizeof(struct up_thread_queue));
    if (thread_queue == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    struct up_concurrentQueue *queue = malloc(sizeof(struct up_concurrentQueue));
    if (queue == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    //struct up_writer_head
    
    struct up_linkElement *link1 = linkElement_alloc();
    if (link1 == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    queue->first[UP_BUFFER_1] = link1;
    queue->last[UP_BUFFER_1] = link1;
    
    
    struct up_linkElement *link2 = linkElement_alloc();
    if (link2 == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    queue->first[UP_BUFFER_2] = link2;
    queue->last[UP_BUFFER_2] = link2;
    
    queue->reader = UP_BUFFER_1;
    //SDL_AtomicSet(&queue->live_chain.writer_head, UP_BUFFER_2);
    queue->writer_head = UP_BUFFER_2;
    
    thread_queue->live_chain = queue;
    
    
    struct up_linkElement *garbage = linkElement_alloc();
    if (garbage == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    thread_queue->garbage.start = garbage;
    thread_queue->garbage.end = garbage;
    
    return thread_queue;
}


// this setup the garbage collection for all the queues
// so every thing can run,
int up_concurrentQueue_start_setup(struct up_thread_queue *queue)
{
    
    return 1;
    
}

void up_concurrentQueue_free(struct up_thread_queue *queue)
{
    struct up_linkElement *tmpLink = NULL;
    struct up_linkElement *current = NULL;
    
    //free buffer 1
    current = queue->live_chain->first[UP_BUFFER_1];
    queue->live_chain->first[UP_BUFFER_1] = NULL;
    
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }
    queue->live_chain->last[UP_BUFFER_1] = NULL;
    
    //free buffer 2
    current = queue->live_chain->first[UP_BUFFER_2];
    queue->live_chain->first[UP_BUFFER_2] = NULL;
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }
    queue->live_chain->last[UP_BUFFER_2] = NULL;
    
    free(queue->live_chain);
    //free the garbage recyler
    current = queue->garbage.start;
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }
    
    free(queue);
    
}


void up_concurrentQueue_shutdown_deinit()
{
        //internal_concurrentQueue = NULL;
}


/**
 Unit test,
 it creates one thread that produces data and one that reads data, at the end
 we check the balance of the buffer to see if it is empty or full
 
 **/

#define UP_UNITTEST_TESTFOR_SECONDS 10

struct up_unitTest_data
{
    int *bat_signal;
    long int numOperWriter;
    long int numOperReader;
};
/*
static int up_unitTest_concurency_queue_producer(void *data)
{
    struct up_unitTest_data *data_ptr = (struct up_unitTest_data*)data;
    
    int *bat_signal = data_ptr->bat_signal;
    
    struct objUpdateInformation local_data;
    //SDL_Delay(200);
    int data_Generated= 0;
    
    printf("\nproducer active\n");
    while (*bat_signal != 1)
    {
        //local_data.id = rand();
        
        if(up_writeToNetworkDatabuffer(&local_data) == 0)
        {
            printf("\nsomething wierd is going on\n");
        }
        data_Generated++;
        
        
    }
    data_ptr->numOperWriter = data_Generated;
    
    printf("\nproducerQueue: %d\n",data_Generated);
    return 0;
}

static int up_unitTest_concurency_queue_consumer(void *data)
{
    struct up_unitTest_data *data_ptr = (struct up_unitTest_data*)data;
    
    int *bat_signal = data_ptr->bat_signal;
    
#define UP_UNITTEST_CONCURENCY_QUEUE_LENGTH 2000
    struct objUpdateInformation local_data[UP_UNITTEST_CONCURENCY_QUEUE_LENGTH];
    int length = UP_UNITTEST_CONCURENCY_QUEUE_LENGTH;
#undef UP_UNITTEST_CONCURENCY_QUEUE_LENGTH
    int data_processed = 0;
    int data_read = 0;
    int failed_read = 0;
    printf("\nconsumer active\n");
    while (*bat_signal != 1)
    {
        
        data_read = up_readNetworkDatabuffer(local_data, length);
        
        if(data_read == 0)
        {
            SDL_Delay(5);
            failed_read++;
        }
        data_processed += data_read;
        
    }
    SDL_Delay(20);
    printf("\nConsumerQueue: reade:%d , queue empty:%d\n",data_processed,failed_read);
    data_ptr->numOperReader = data_processed;    return 0;
}

static void up_unitTest_concr_queue_result(struct up_unitTest_data *data)
{
    long int data_processed = data->numOperReader;
    long int data_Generated = data->numOperWriter;
    double bytePerObj = sizeof(struct up_linkElement);
    printf("\nObj size: %f\n",bytePerObj);
    
    printf("\nObj transferd total:\n  into: %ld\n   out: %ld\n",data_Generated,data_processed);
    
    double objRead = data_processed;
    double gigRead =(double)(objRead)/(1000 * 1000 *1000);
    double objWrite = data_Generated;
    double gigWrite =(double)(objWrite)/(1000 * 1000 *1000);
    
    
    printf("  into: %f MWrite/tot\n   out: %f MRead/tot\n",gigWrite*1000,gigRead*1000);
    
    double gigReadTot = gigRead * bytePerObj;
    double gigWriteTot = gigWrite * bytePerObj;
    
    printf("\nObj transferd Byte:\n  into: %f GByte/tot\n   out: %f GByte/tot\n",gigWriteTot,gigReadTot);
    
    
    double gigReadPerSec = gigReadTot/UP_UNITTEST_TESTFOR_SECONDS;
    double gigWriterPerSec = gigWriteTot/UP_UNITTEST_TESTFOR_SECONDS;
    
    printf("  into: %f GByte/s\n   out: %f GByte/s\n",gigWriterPerSec,gigReadPerSec);
    
    
    printf("\nObj sum:\n");
    printf("  Total: %f MTransaction/tot\n",(gigWrite + gigRead)*1000);
    printf("  Total: %f GByte/tot\n",gigWriteTot + gigReadTot);
    printf("  Total: %f GByte/s\n",gigWriterPerSec + gigReadPerSec);
    
    
    double gigOprReadPerSec = gigRead/UP_UNITTEST_TESTFOR_SECONDS;
    double gigOprWriterPerSec = gigWrite/UP_UNITTEST_TESTFOR_SECONDS;
    printf("\nData (Gbyte/s):\n");
    printf("        obj size:%d        obj size:64        obj size:56\n",(int)bytePerObj);
    printf(" Write: %f Gbyte/s  %f Gbyte/s  %f Gbyte/s\n",gigOprWriterPerSec * bytePerObj,gigOprWriterPerSec * 64,gigOprWriterPerSec*56);
    printf("  read: %f Gbyte/s  %f Gbyte/s  %f Gbyte/s\n",gigOprReadPerSec * bytePerObj,gigOprReadPerSec*64,gigOprReadPerSec*56);
    printf(" total: %f Gbyte/s  %f Gbyte/s  %f Gbyte/s\n",(gigOprWriterPerSec + gigOprReadPerSec)*bytePerObj,(gigOprWriterPerSec + gigOprReadPerSec)*64,(gigOprWriterPerSec + gigOprReadPerSec)*56);
    
}*/

void up_unitTest_concurency_queue_spsc()
{
    /*srand(52352); // dont actualy need a random value, just a diffrent values
    //Pthread_listen_datapipe listen_pipe;
    SDL_Thread *producer = NULL;
    SDL_Thread *consumer = NULL;
    int bat_signal = 0; // =)
    
    struct up_unitTest_data data;
    data.bat_signal = &bat_signal;
    data.numOperReader = 0;
    data.numOperWriter = 0;
    
    up_concurrentQueue_start_setup();
    SDL_Delay(1000);
    
    producer=SDL_CreateThread(up_unitTest_concurency_queue_producer,"producer_queue_test",&data);
    if(producer==NULL){
        UP_ERROR_MSG_STR("create_thread faild", SDL_GetError());
        
    }
    
    consumer=SDL_CreateThread(up_unitTest_concurency_queue_consumer,"consumer_queue_test",&data);
    if(consumer==NULL){
        UP_ERROR_MSG_STR("create_thread faild", SDL_GetError());
        
    }
    SDL_Delay(UP_UNITTEST_TESTFOR_SECONDS*1000);
    bat_signal = 1;
    
    SDL_WaitThread(producer, NULL);
    SDL_WaitThread(consumer, NULL);
    
    up_concurrentQueue_shutdown_deinit();
    up_unitTest_concr_queue_result(&data);*/
}


