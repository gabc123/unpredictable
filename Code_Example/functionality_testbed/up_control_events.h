//
//  up_control_events.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef __unpredictable_xcode__up_control_events__
#define __unpredictable_xcode__up_control_events__
#include "up_sdl_redirect.h"
#include "up_object_actions.h"


//Tobias
struct cooldownTimer
{
    unsigned int startTime;
    unsigned int coolDown;
    unsigned int ammunitionSpeed;
    int ammunition;
    int damage;
};
//Tobias
struct up_shootingFlag
{
    struct cooldownTimer bulletFlag;
    struct cooldownTimer missileFlag;
    struct cooldownTimer laserFlag;
    
};
//Tobias
struct up_eventState
{
    struct up_shootingFlag flags;
};

//Magnus
struct up_key_map
{
    char name[25];
    SDL_Keycode key;
    void (*keyDown_func)(struct up_eventState *currentEvent,struct up_actionState *objectAction);
    void (*keyUp_func)(struct up_eventState *currentEvent,struct up_actionState *objectAction);
};

struct up_key_map *up_key_remapping_setup();

//handles keyinputs
int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction,struct up_key_map *up_keymap);

#endif /* defined(__unpredictable_xcode__up_control_events__) */
