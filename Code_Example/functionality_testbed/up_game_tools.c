//
//  up_game_tools.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_game_tools.h"
#include "up_sdl_redirect.h"


static double up_gFrameTickRate = 0;
static unsigned int up_gFramePerSeconde = 0;

unsigned int up_getFrameRate()
{
    return up_gFramePerSeconde;
}

double up_getFrameTimeDelta()
{
    return up_gFrameTickRate;
}

//Magnus
void up_updateFrameTickRate()
{
    static unsigned int fps_counter = 0;
    static unsigned int lastTick = 0;
    static unsigned int startTick = 0;
    
    //how many milliseconds have pased between frames
    double diffTick = SDL_GetTicks() - lastTick;
    
    // this results in fraction of a seconde that passed between frames
    // used to give smooth movment regardless of fps
    up_gFrameTickRate = diffTick/1000.0;
    
    //When a entire seconde have elapsed print the frame per seconds
    if ((SDL_GetTicks() - startTick) > 1000) {
        startTick =  SDL_GetTicks();
        up_gFramePerSeconde = fps_counter;
        //printf("FPS: %d , diffTick: %f globalTickRate: %f\n",up_gFramePerSeconde,diffTick,up_gFrameTickRate);
        fps_counter = 0;
    }
    
    fps_counter++;
    lastTick = SDL_GetTicks();
}


// magnus, used to give system informations,
void up_system_check()
{
    
    printf("\nSize of char : %lu",sizeof(char));
    printf("\nSize of int: %lu",sizeof(int));
    printf("\nSize of long: %lu",sizeof(long));
    printf("\nSize of float %lu\n",sizeof(float));
}