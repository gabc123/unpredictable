//
//  up_control_events.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_control_events.h"
#include "up_camera_module.h"


//Tobias
// magnus , ammocheck
int checkFire(struct cooldownTimer weapon)
{
    unsigned int tempVar=SDL_GetTicks()-weapon.startTime;
    
    if (tempVar>= weapon.coolDown && weapon.ammunition > 0)
    {
        return 1;
    }
    return 0;
}

//Magnus
/**
 Key remapping
 The key_map struct contains 2 function pointers, one for key down , and one for key up
 also what key they map to.
 below are all the functions
 At the bottom is the main function
 **/


//no operation
void up_key_function_noop(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    
}

//movement
void up_key_function_forward(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->engine.state = fwd;
}
void up_key_function_backward(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->engine.state = bwd;
}
void up_key_function_turnLeft(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->maneuver.state = left;
}
void up_key_function_turnRigth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->maneuver.state = right;
}
//Rotate
void up_key_function_bankRigth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->maneuver.state = bankRight;
}
void up_key_function_bankLeft(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    objectAction->maneuver.state = bankLeft;
}
//Zooming
void up_key_function_zoomin(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_cam_zoom(1.0f);
}
void up_key_function_zoomout(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_cam_zoom(-1.0f);
}
// demonstration code to toggle different properties
//Magnus
#ifdef UP_PRESENTATION_MODE
void up_key_function_toggle_wireframe(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_toggle_wireframe();
}
void up_key_function_toggle_ambient(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_toggle_ambientLigth();
}
void up_key_function_toggle_dirligth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_toggle_directionalLigth();
}
void up_key_function_toggle_ligth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    up_toggle_allLigth();
}
#endif
//fire main weapon for playership
void up_key_function_firebullet(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    //Checks if its ok to fire a projectile
    int tempFlag = checkFire(currentEvent->flags.bulletFlag);
    if(tempFlag==1)
    {
        objectAction->fireWeapon.state = fireBullet;
        currentEvent->flags.bulletFlag.startTime = SDL_GetTicks();  //marks the time the wepon was fired,
    }else
    {
        objectAction->fireWeapon.state = none;
    }
}
//missile
void up_key_function_firemissile(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    int tempFlag = checkFire(currentEvent->flags.missileFlag);
    if(tempFlag==1)
    {
        objectAction->fireWeapon.state = fireMissile;
        currentEvent->flags.missileFlag.startTime = SDL_GetTicks();
    }else
    {
        objectAction->fireWeapon.state = none;
    }
}
//lazer
void up_key_function_firelaser(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    int tempFlag = checkFire(currentEvent->flags.laserFlag);
    if(tempFlag==1)
    {
        objectAction->fireWeapon.state = fireLaser;
        currentEvent->flags.laserFlag.startTime = SDL_GetTicks();
    }else
    {
        objectAction->fireWeapon.state = none;
    }
}

void up_key_function_stop_forward(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->engine.state == fwd)
    {
        objectAction->engine.none=none;
    }
}

void up_key_function_stop_backward(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->engine.state == bwd)
    {
        objectAction->engine.none=none;
    }
}

void up_key_function_stop_turnrigth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->maneuver.state == right)
    {
        objectAction->maneuver.none=none;
    }
}

void up_key_function_stop_turnleft(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->maneuver.state == left)
    {
        objectAction->maneuver.none=none;
    }
}

void up_key_function_stop_firebullet(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->fireWeapon.state == fireBullet)
    {
        objectAction->fireWeapon.none=none;
    }
}

void up_key_function_stop_bankrigth(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->maneuver.state == bankRight)
    {
        objectAction->maneuver.none = none;
    }
}

void up_key_function_stop_bankleft(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{
    if(objectAction->maneuver.state == bankLeft)
    {
        objectAction->maneuver.none = none;
    }
}


/**
 keymapp
 
 */

// Name , what key activates it, function for keyDown event, function for keyUp event
//Magnus
struct up_key_map internal_keymap[] =
{{"Move forward",SDLK_w                      , &up_key_function_forward , &up_key_function_stop_forward},
    {"Move backward",SDLK_s                  , &up_key_function_backward , &up_key_function_stop_backward},
    {"Turn left",SDLK_a                 , &up_key_function_turnLeft , &up_key_function_stop_turnleft},
    {"Turn rigth",SDLK_d                , &up_key_function_turnRigth , &up_key_function_stop_turnrigth},
    {"Bank left",SDLK_q                 , &up_key_function_bankLeft , &up_key_function_stop_bankleft},
    {"Bank right",SDLK_e                , &up_key_function_bankRigth, &up_key_function_stop_bankrigth},
    {"Zoom in",SDLK_r                   , &up_key_function_zoomin, &up_key_function_noop},
    {"Zoom out",SDLK_f                  , &up_key_function_zoomout, &up_key_function_noop},
    // demonstration code to toggle diffrent propertys
#ifdef UP_PRESENTATION_MODE
    {"Toggle wireframe",SDLK_t          , &up_key_function_toggle_wireframe, &up_key_function_noop},
    {"Toggle amb. ligth",SDLK_y      , &up_key_function_toggle_ambient, &up_key_function_noop},
    {"Toggle dir. ligth",SDLK_u  , &up_key_function_toggle_dirligth, &up_key_function_noop},
    {"Toggle ligth",SDLK_i              , &up_key_function_toggle_ligth, &up_key_function_noop},
#endif
    {"Fire bullet",SDLK_SPACE           , &up_key_function_firebullet, &up_key_function_stop_firebullet},
    {"Fire missile",SDLK_c              , &up_key_function_firemissile, &up_key_function_noop},
    {"Fire laser",SDLK_v                , &up_key_function_firelaser, &up_key_function_noop},
    {"\0",0,&up_key_function_noop,&up_key_function_noop}};  //end

struct up_key_map *up_key_remapping_setup()
{
    return &internal_keymap[0];
}


// tobias
// magnus added keyremappin
int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction,struct up_key_map *up_keymap)
{
    int flag = 1;
    SDL_Event event;
    SDL_Keycode currentKey;
    int i = 0;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flag = 0;
        }
        if(event.type == SDL_KEYDOWN) {
            currentKey = event.key.keysym.sym;
            for (i = 0; up_keymap[i].key != 0; i++) {
                if (up_keymap[i].key == currentKey) {
                    up_keymap[i].keyDown_func(currentEvent,objectAction);
                    break;
                }
            }
        }
        
        if(event.type == SDL_KEYUP)
        {
            currentKey = event.key.keysym.sym;
            for (i = 0; up_keymap[i].key != 0; i++) {
                if (up_keymap[i].key == currentKey) {
                    up_keymap[i].keyUp_func(currentEvent,objectAction);
                    break;
                }
            }
        }
    }
    //shipMove(movement, ship);
    return flag;
}
