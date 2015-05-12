//
//  main.c
//  server_test_udp
//
//  Created by o_0 on 2015-05-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include <stdio.h>
#include "up_server.h"
#include "up_sdl_redirect.h"
#include "up_error.h"
#include "up_thread_utilities.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    /*if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        UP_ERROR_MSG_STR("SDL_INIT failed, we are all doomed!!\n",SDL_GetError());
    }*/
    
    struct internal_server_state *server_state = up_server_startup();
    up_server_run(server_state);
    up_server_shutdown_cleanup(server_state);
    
    //up_server_start();
    //up_unitTest_concurency_queue_spsc();
    
    return 0;
}
