#include "up_concurrent_test.h"


#define UP_BUFFER_1 0
#define UP_BUFFER_2 1

/*
    
 It became obvious that the cpu only loaded the page that was going to be changed when read/write was
 set to off ( that means only the id value is copyed from the structure)
    1048520 12212.520564
    131016 1601.869985
    65480 1032.629944
    32712 587.811458
    16328 314.034412
    
    diffrent  bandwidth: blockalloc 2048, padding 8136 , delay 5 , fullcopy read/write off, 156.644916 GB/s
    diffrent  bandwidth: blockalloc 2048, padding 4040 , delay 5 , fullcopy read/write off, 78.891270 GB/s
    diffrent  bandwidth: blockalloc 2048, padding 1992 , delay 5 , fullcopy read/write off, 47,532985 GB/s
 
    diffrent bandwidth: blockalloc 8160, padding 968 , delay 5, fullcopy read/write off: 30.9799 GB/s
 
still doing a full write, but only the id is read, the cpu has a max L2 read rate of 40 GB/s i think
    diffrent bandwidth: blockalloc 8160, padding 968 , delay 5, fullcopy read off : 30.0786 GB/s
 
    diffrent bandwidth: blockalloc 8160, padding 968 , delay 5, fullcopy read on  : 17.89915 GB/s
 
    other: blockalloc 8160, standard, fullcopy off: 5.0445 GB/s
    other:  blockalloc 8160, standard, fullcopy read off: 1.85 GB/s
 */
#define UP_UNITTEST_BLOCKALLOC_SIZE 1024
#define UP_UNITTEST_PACKET_PADDING_SIZE 440
#define UP_UNITTEST_READ_DELAY 5

#define UP_UNITTEST_FULL_COPY_READ
#define UP_UNITTEST_FULL_COPY_WRITE


#define UP_BLOCKALLOC_TEST
//#define UP_CACHLINE_TEST
//#define UP_STANDARD_TEST
//#define UP_MIDDLEPADDING_TEST
//#define UP_SMALLOBJ_TEST

#ifdef UP_CACHLINE_TEST
struct up_linkElement
{
    struct up_linkElement *next;
    struct objUpdateInformation obj;
}__attribute__ ((aligned (64)));
#else //
#ifdef UP_STANDARD_TEST
struct up_linkElement
{
    struct up_linkElement *next;
    struct objUpdateInformation obj;
};
#else
#ifdef UP_MIDDLEPADDING_TEST
struct up_linkElement
{
    struct up_linkElement *next;
    char dummyObj[UP_UNITTEST_PACKET_PADDING_SIZE];
    struct objUpdateInformation obj;
};
#else
#ifdef UP_SMALLOBJ_TEST
struct small_dummy_test
{
    int id;
};

struct up_linkElement
{
    struct up_linkElement *next;
    struct small_dummy_test obj;
};//__attribute__ ((aligned (768)));
#else

struct up_linkElement
{
    struct up_linkElement *next;
    struct objUpdateInformation obj;
    char dummyObj[UP_UNITTEST_PACKET_PADDING_SIZE];
};//__attribute__ ((aligned (768)));
#endif
#endif
#endif
#endif
//struct up_writer_head {
//    struct up_linkElement *head;
//    int buffer_loc;
//};


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


static struct up_concurrentQueue *internal_concurrentQueue = NULL;


static struct up_linkElement *linkElement_alloc();
static struct up_linkElement *linkElement_create();
static void linkElement_recycle(struct up_linkElement * fromLink,struct up_linkElement * toLink);


// lockfree waitfree reader on a buffer
int up_readNetworkDatabuffer(struct objUpdateInformation *data,int length)
{
    // move the writer to the other buffer
    int reader = internal_concurrentQueue->reader;
    struct up_linkElement *first = NULL;
    struct up_linkElement *last = NULL;
    struct up_linkElement *tmpLink = NULL;
    
    //reader = internal_concurrentQueue->reader;
    // change the writerhead atomicly so the writer will work on the other
    // linklist buffer,(it do not matter if the last node change between operations,
    // the writer look for the end always, it only matter that it is set atmoicly )
    
    //SDL_AtomicSet(&internal_concurrentQueue->writer_head, reader);
    internal_concurrentQueue->writer_head = reader;
    
    // change the reader variable so it matches current state
    reader = (reader == UP_BUFFER_1) ? UP_BUFFER_2 : UP_BUFFER_1;
    
    first = internal_concurrentQueue->first[reader];
    last = internal_concurrentQueue->last[reader];
    
    // it is empty
    if (first == last || first == NULL) {
        internal_concurrentQueue->reader = reader;  //change the reader position so we can get data on other que
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
        //weired
    }
    // if current == last, then the while loop never gets executed
    // so this make sure this is not null at the end of the function.
    tmpLink = first;
    
    // walk the linklist until the last link
    // notice that the last element never gets reaped
    // this is becouse we do not want to have a empty queue,
    // some one needs to be first/last to be able to connect to
    while ((current->next != NULL) && (count < length) ) {
        tmpLink = current;
#ifdef UP_UNITTEST_FULL_COPY_READ
        data[count] = tmpLink->obj;
#else
        data[count].id = tmpLink->obj.id;
#endif
        count++;
        current = tmpLink->next;
        
    }
    
    
    
    // move the first pointer to the correct location for continued reading
    internal_concurrentQueue->first[reader] = current;
    int countRead = count;
    count++; // walk to the last element
    //if the whole buffer gets consumed we need to set a new last link
    if (count < length) {
#ifdef UP_UNITTEST_FULL_COPY_READ
        data[count] = current->obj;
#else
        data[count].id = current->obj.id;
#endif
        current->obj.id = 0; // id 0 is a dummy id for no data to be read
        //we set the new last link so we can attache the writer on the correct spot
        internal_concurrentQueue->last[reader] = current;
        
        countRead = count;
        
        // notice that we do not update reader if we run out of data space
        // this is only done if we intende to change queue nexte cycle
        internal_concurrentQueue->reader = reader;
    }
    
 
    // this repurpouse the link chain to be used again
    // reducing memory overhead, and waste
    linkElement_recycle(first,tmpLink);
    
    return countRead; //succsess
}

int up_writeToNetworkDatabuffer(struct objUpdateInformation *data)
{
    struct up_linkElement *newData = linkElement_create();
    if (newData == NULL) {
        return 0;
    }
#ifdef UP_UNITTEST_FULL_COPY_WRITE
    newData->obj = *data;
#else
    newData->obj.id = data->id;
#endif
    newData->next = NULL;
    struct up_linkElement *currentLast = NULL;
    int whatBuffer = -1;
    int check = 0;
    int counter_failed = 0;
    do {
        /*if (whatBuffer != (check = SDL_AtomicGet(&internal_concurrentQueue->writer_head))) {
            currentLast = internal_concurrentQueue->last[check];
            whatBuffer = check;
        }else
        {
            currentLast = currentLast->next;
        }*/
        if (whatBuffer != ((check = internal_concurrentQueue->writer_head))) {
            currentLast = internal_concurrentQueue->last[check];
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
        
    }while (SDL_AtomicCASPtr((void *)&currentLast->next, NULL, newData) == SDL_FALSE);

    internal_concurrentQueue->last[whatBuffer] = newData;
    return 1;
}



struct up_concurrentQueue_gabageRecycler
{
    struct up_linkElement *start;
    struct up_linkElement *end;
};

struct up_concurrentQueue_gabageRecycler internal_queueRecycler;

#ifdef UP_BLOCKALLOC_TEST
static struct up_linkElement *linkElement_blockAlloc(int blockSize);
#endif

static struct up_linkElement *linkElement_alloc()
{
#ifndef UP_BLOCKALLOC_TEST
    struct up_linkElement *link = malloc(sizeof(struct up_linkElement));
    if (link == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    link->next = NULL;
    link->obj.id = 0;
    return link;
#else
    struct up_linkElement *link = linkElement_blockAlloc(UP_UNITTEST_BLOCKALLOC_SIZE);
    //observ idx 1 and not 0, so we can use that element
    internal_queueRecycler.end->next = &link[1];
    internal_queueRecycler.end = &link[UP_UNITTEST_BLOCKALLOC_SIZE - 1];
    
    link->next = NULL;
    return link;
#endif
}



// consume the recyled list if there is anyone
// else it will allocate a new link
static struct up_linkElement *linkElement_create()
{
    
    struct up_linkElement *link = internal_queueRecycler.start;
    if (link == NULL) {
        return linkElement_alloc();
    }
    
    
    //we do not want to return the last element
    if (internal_queueRecycler.start == internal_queueRecycler.end)
    {
        return linkElement_alloc();
    }
    
    
    //when a link gets recycle there is a chance that the end link has not been updated
    // in that case we check to se if last , if so alloc a new linkelement
    if (link->next == NULL) {
        return linkElement_alloc();
    }
    
    // now we can relink the list and return our recycled link
    internal_queueRecycler.start = link->next;
    link->next = NULL; //decouple link from this
    return link;
}

//#define UP_PRODUCTION
// we have a linklist of unused links,
// when a link is not needed any more the next element
// cpu reordering information: http://www.rdrop.com/users/paulmck/scalability/paper/ordering.2007.09.19a.pdf
//
static void linkElement_recycle(struct up_linkElement * fromLink,struct up_linkElement * toLink)
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
    SDL_AtomicSetPtr((void *)&toLink->next, NULL);  //decouple this link chain from the buffer

    SDL_MemoryBarrierRelease();
    
    // connect the chain to the recycler for use
    internal_queueRecycler.end->next = fromLink; // !! must happen after tolink->next = NULL
    
    internal_queueRecycler.end = toLink;
}

struct internal_linkBLock
{
    struct internal_linkBLock *next;
    struct up_linkElement *memblob;
};

struct internal_linkBLock *internal_blockTest_start = NULL;
struct internal_linkBLock *internal_blockTest_end = NULL;


struct up_linkElement *linkElement_blockAlloc(int blockSize)
{
    struct internal_linkBLock *blockptr = malloc(sizeof(struct internal_linkBLock));
    if (blockptr == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    
    
    struct up_linkElement *link = malloc(sizeof(struct up_linkElement)*blockSize);
    if (link == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    link[blockSize -1 ].next = NULL;
    link[blockSize -1 ].obj.id = 0;
    struct up_linkElement *current = &link[0];
    int i = 0;
    for (i = 1; i < blockSize; i++) {
        current->next = &link[i];
        current->obj.id = 0;
        current = current->next;
    }
    
    if (internal_blockTest_end == NULL) {
        blockptr->memblob = link;
        blockptr->next = NULL;
        internal_blockTest_end = blockptr;
        internal_blockTest_start = blockptr;
        return link;
    }
    blockptr->next = NULL;
    blockptr->memblob = link;
    internal_blockTest_end->next = blockptr;
    internal_blockTest_end = blockptr;
    
    return link;
}

int up_concurrentQueue_start_setup()
{
    internal_concurrentQueue = malloc(sizeof(struct up_concurrentQueue));
    if (internal_concurrentQueue == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
#ifndef UP_BLOCKALLOC_TEST
    struct up_linkElement *garbage = linkElement_alloc();
    if (garbage == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    internal_queueRecycler.start = garbage;
    internal_queueRecycler.end = garbage;
#else
    
    struct up_linkElement *garbage = linkElement_blockAlloc(UP_UNITTEST_BLOCKALLOC_SIZE);
    if (garbage == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    internal_queueRecycler.start = &garbage[0];
    internal_queueRecycler.end = &garbage[UP_UNITTEST_BLOCKALLOC_SIZE - 1];
#endif
    
    //struct up_writer_head
    struct up_linkElement *link1 = linkElement_alloc();
    if (link1 == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    internal_concurrentQueue->first[UP_BUFFER_1] = link1;
    internal_concurrentQueue->last[UP_BUFFER_1] = link1;
    
    
    struct up_linkElement *link2 = linkElement_alloc();
    if (link2 == NULL) {
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    internal_concurrentQueue->first[UP_BUFFER_2] = link2;
    internal_concurrentQueue->last[UP_BUFFER_2] = link2;
    
    internal_concurrentQueue->reader = UP_BUFFER_1;
    //SDL_AtomicSet(&internal_concurrentQueue->writer_head, UP_BUFFER_2);
    internal_concurrentQueue->writer_head = UP_BUFFER_2;
    
    return 1;
    
}


void up_concurrentQueue_shutdown_deinit()
{
#ifndef UP_BLOCKALLOC_TEST
    struct up_linkElement *tmpLink = NULL;
    struct up_linkElement *current = NULL;
    
    //free buffer 1
    current = internal_concurrentQueue->first[UP_BUFFER_1];
    internal_concurrentQueue->first[UP_BUFFER_1] = NULL;
    
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }
    internal_concurrentQueue->last[UP_BUFFER_1] = NULL;
    
    //free buffer 2
    current = internal_concurrentQueue->first[UP_BUFFER_2];
    internal_concurrentQueue->first[UP_BUFFER_2] = NULL;
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }
    internal_concurrentQueue->last[UP_BUFFER_2] = NULL;
    
    //free the garbage recyler
    current = internal_queueRecycler.start;
    while (current != NULL) {
        tmpLink = current;
        current = current->next;
        tmpLink->next = NULL;   //break link
        free(tmpLink);
    }

    free(internal_concurrentQueue);
    internal_concurrentQueue = NULL;

#else
    struct internal_linkBLock *blockptr = internal_blockTest_start;
    struct internal_linkBLock *current = internal_blockTest_start;
    struct up_linkElement *tmpLink = NULL;
    

    while (current != NULL) {
        blockptr = current;
        current = current->next;
        blockptr->next = NULL;   //break link
        tmpLink = blockptr->memblob;
        free(tmpLink);
        free(blockptr);
    }
    
    free(internal_concurrentQueue);
    internal_concurrentQueue = NULL;
    
#endif
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
        local_data.id++;
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
            SDL_Delay(UP_UNITTEST_READ_DELAY);
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

}

void up_unitTest_concurency_queue_spsc()
{
    srand(52352); // dont actualy need a random value, just a diffrent values
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
    
    consumer=SDL_CreateThread(up_unitTest_concurency_queue_consumer,"producer_queue_test",&data);
    if(consumer==NULL){
        UP_ERROR_MSG_STR("create_thread faild", SDL_GetError());
        
    }
    SDL_Delay(UP_UNITTEST_TESTFOR_SECONDS*1000);
    bat_signal = 1;
    
    SDL_WaitThread(producer, NULL);
    SDL_WaitThread(consumer, NULL);
    
    up_concurrentQueue_shutdown_deinit();
    up_unitTest_concr_queue_result(&data);
}





