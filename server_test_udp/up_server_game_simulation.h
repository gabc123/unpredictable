//
//  up_server_game_simulation.h
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-26.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_SERVER_GAME_SIMULATION_H
#define UP_SERVER_GAME_SIMULATION_H
#include "up_thread_utilities.h"

struct up_interThread_communication
{
    int *shutdown_signal; // points to a shared shutdown signal
    struct up_thread_queue *simulation_input;   // connected to the recive thread
    struct up_thread_queue *simulation_output;  //connected to the send thread
};


#endif /* defined(__up_server_part_xcode__up_server_game_simulation__) */
