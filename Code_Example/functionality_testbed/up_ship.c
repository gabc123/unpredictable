#include "up_ship.h"
#include "up_error.h"
#include "up_sdl_redirect.h"
#include "up_utilities.h"
#include "up_shader_module.h"
#include "up_camera_module.h"
#include "up_modelRepresentation.h"
#include "up_render_engine.h"
#include "up_filereader.h"
#include "up_assets.h"
#include "up_music.h"
#include <math.h>
#define NAMESIZE 100


// magnus , up_updateMovements , 5 maj

double up_getFrameTimeDelta();

double up_gFrameTickRate = 0;
unsigned int up_gFramePerSeconde = 0;

int checkFire(unsigned int startTime, unsigned int cooldown);


//not it use
//Sebastian
//Magnus
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship){
    float deltaTime = (float)up_getFrameTimeDelta();
    ship->speed += 1.0f *(movement->up - movement->down) * deltaTime;
    ship->angle += 1.f *(movement->left - movement->right) * deltaTime;
    if(!(movement->up + movement->down)){ship->speed=0;}
}





/**
    Key remapping
    The key_map struct contains 2 function pointers, one for key down , and one for key up
    also what key they map to.
    below are all the functions

 **/




void up_key_function_noop(struct up_eventState *currentEvent,struct up_actionState *objectAction)
{

}

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
// demonstration code to toggle diffrent propertys
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
    int tempFlag = checkFire(currentEvent->flags.bulletFlag.startTime, currentEvent->flags.bulletFlag.coolDown);
    if(tempFlag==0)
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
    int tempFlag = checkFire(currentEvent->flags.missileFlag.startTime, currentEvent->flags.missileFlag.coolDown);
    if(tempFlag==0)
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
    int tempFlag = checkFire(currentEvent->flags.laserFlag.startTime, currentEvent->flags.laserFlag.coolDown);
    if(tempFlag==0)
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
struct up_key_map internal_keymap[] =
{{"Forward",SDLK_w                      , &up_key_function_forward , &up_key_function_stop_forward},
    {"Backward",SDLK_s                  , &up_key_function_backward , &up_key_function_stop_backward},
    {"Turn Left",SDLK_a                 , &up_key_function_turnLeft , &up_key_function_stop_turnleft},
    {"Turn Rigth",SDLK_d                , &up_key_function_turnRigth , &up_key_function_stop_turnrigth},
    {"Bank Left",SDLK_q                 , &up_key_function_bankLeft , &up_key_function_stop_bankleft},
    {"Bank Right",SDLK_e                , &up_key_function_bankRigth, &up_key_function_stop_bankrigth},
    {"Zoom in",SDLK_r                   , &up_key_function_zoomin, &up_key_function_noop},
    {"Zoom out",SDLK_f                  , &up_key_function_zoomout, &up_key_function_noop},
    // demonstration code to toggle diffrent propertys
#ifdef UP_PRESENTATION_MODE
    {"Toggle wireframe",SDLK_t          , &up_key_function_toggle_wireframe, &up_key_function_noop},
    {"Toggle ambiant Ligth",SDLK_y      , &up_key_function_toggle_ambient, &up_key_function_noop},
    {"Toggle Directional Ligth",SDLK_u  , &up_key_function_toggle_dirligth, &up_key_function_noop},
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
//Tobias
int checkFire(unsigned int startTime, unsigned int cooldown)
{
   unsigned int tempVar=SDL_GetTicks()-startTime;

   if (tempVar<=cooldown)
   {
       return 1;
   }
   return 0;
}

unsigned int up_getFrameRate()
{
    return up_gFramePerSeconde;
}

double up_getFrameTimeDelta()
{
    return up_gFrameTickRate;
}

//Sebastian 2015-05-15
void up_handleCollision(struct up_allCollisions *allcollisions)
{
    int i=0;
    struct up_objectInfo *object1;
    struct up_objectInfo *object2;

    for(i=0; i < allcollisions->nrShipEnviroment; i++){
        object1 = up_unit_objAtIndex(up_ship_type, allcollisions->shipEnviroment[i].object1);
        object2 = up_unit_objAtIndex(up_environment_type, allcollisions->shipEnviroment[i].object2);
        if (object1 == NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }

        object2->dir=object1->dir;
        object2->pos.x+=5*object1->dir.x;
        object2->pos.y+=5*object1->dir.y;
        object2->speed=object1->speed*3/4;
        object1->speed =object1->speed/2;

    }
    for(i=0; i < allcollisions->nrProjectileEnviroment; i++){
        object1 = up_unit_objAtIndex(up_projectile_type, allcollisions->projectileEnviroment[i].object1);
        object2 = up_unit_objAtIndex(up_environment_type, allcollisions->projectileEnviroment[i].object2);
        if (object1 == NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }

        object2->dir=object1->dir;
        object2->pos.x+=5*object1->dir.x;
        object2->pos.y+=5*object1->dir.y;
        object2->speed=object1->speed*3/4;
        object1->speed =object1->speed/2;

    }

    for(i=0; i < allcollisions->nrProjectileShip; i++){
        object1 = up_unit_objAtIndex(up_projectile_type, allcollisions->projectileShip[i].object1);
        object2 = up_unit_objAtIndex(up_ship_type, allcollisions->projectileShip[i].object2);
        if (object1 == NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }

        if (object2 ==NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }

        object2->dir=object1->dir;
        object2->pos.x+=5*object1->dir.x;
        object2->pos.y+=5*object1->dir.y;
        object2->speed=object1->speed*3/4;
        object1->speed =object1->speed/2;

    }

    for(i=0; i < allcollisions->nrEnviromentEnviroment; i++){
        object1 = up_unit_objAtIndex(up_environment_type, allcollisions->enviromentEnviroment[i].object1);
        object2 = up_unit_objAtIndex(up_environment_type, allcollisions->enviromentEnviroment[i].object2);
        if (object1 == NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }

        if (object2 ==NULL){
            UP_ERROR_MSG_STR("tried accesing nonexisting object\n",SDL_GetError());
            printf("itemnr: %d\n", i);
            continue;
        }
        if(object1->objectId.idx != object2->owner){
            object2->owner = object1->objectId.idx;
            printf("trolololooooo\n");
            object2->dir=object1->dir;
            object2->pos.x+=5*object1->dir.x;
            object2->pos.y+=5*object1->dir.y;
            object2->speed=object1->speed*3/4;
            object1->speed =object1->speed/2;
        }
    }

    /*
    enum up_object_type
    {
        up_ship_type,
        up_projectile_type,
        up_environment_type,
        up_others_type
    };
    */
}

//Sebastian + Tobias 2015-05-12
//checks objects collisionboxes too see whether a hit has occured or not
void testCollision(struct up_objectInfo *object1, struct up_objectInfo *object2, int nrObj1, int nrObj2, struct up_allCollisions *allcollisions, int typeCollision)
{
    float xlengthModel1, ylengthModel1, zlengthModel1;
    float xlengthModel2, ylengthModel2, zlengthModel2;
    float distanceX, distanceY, distanceZ;

    struct Hitbox hitModel1 ={
        object1[nrObj1].pos.x+1.0, object1[nrObj1].pos.y+1.0, object1[nrObj1].pos.z+1.0,
        object1[nrObj1].pos.x-1.0,  object1[nrObj1].pos.y-1.0,  object1[nrObj1].pos.z-1.0};

    struct Hitbox hitModel2 = {
        object2[nrObj2].pos.x+1.0, object2[nrObj2].pos.y+1.0, object2[nrObj2].pos.z+1.0,
        object2[nrObj2].pos.x-1.0,  object2[nrObj2].pos.y-1.0,  object2[nrObj2].pos.z-1.0};

    xlengthModel1 = hitModel1.xmax - hitModel1.xmin;
    ylengthModel1 = hitModel1.ymax - hitModel1.ymin;
    zlengthModel1 = hitModel1.zmax - hitModel1.zmin;

    xlengthModel2 = hitModel2.xmax - hitModel2.xmin;
    ylengthModel2 = hitModel2.ymax - hitModel2.ymin;
    zlengthModel2 = hitModel2.zmax - hitModel2.zmin;

    distanceX = fabsf(object2[nrObj2].pos.x - object1[nrObj1].pos.x);
    distanceY = fabsf(object2[nrObj2].pos.y - object1[nrObj1].pos.y);
    distanceZ = fabsf(object2[nrObj2].pos.z - object1[nrObj1].pos.z);

    //checks if the collisionboxes collides
    if(distanceX <= xlengthModel1 || distanceX <= xlengthModel2)
        if(distanceY <= ylengthModel1 ||distanceY <= ylengthModel2)
            if(distanceZ <= zlengthModel1 || distanceZ <= zlengthModel2)
            {
                //stores the id, arrayplacement by the type of collision
                switch(typeCollision)
                {
                case shipEnviroment:
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment].object1 = object1[nrObj1].objectId.idx;
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment].nrObj1 = nrObj1;
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment].object2 = object2[nrObj2].objectId.idx;
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment++].nrObj2 = nrObj2;
                /*    printf("shipEnviroment\n");
                    printf("object1Id read: %d\n", object1[nrObj1].objectId.idx);
                    printf("object2Id read: %d\n", object2[nrObj2].objectId.idx);
                    printf("object1id stored: %d\n", allcollisions->shipEnviroment[allcollisions->nrShipEnviroment-1].object1);
                    printf("object2id stored: %d\n", allcollisions->shipEnviroment[allcollisions->nrShipEnviroment-1].object2);
                */
                    break;
                //projectile enviroment
                case projectileEnviroment:
                    //printf("projectileEnviroment\n");
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment].object1 = object1[nrObj1].objectId.idx;
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment].nrObj1 = nrObj1;
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment].object2 = object2[nrObj2].objectId.idx;
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment++].nrObj2 = nrObj2;
                    break;
                //projectile ship
                case projectileShip:
                    //printf("projectileShip\n");
                    allcollisions->projectileShip[allcollisions->nrProjectileShip].object1 = object1[nrObj1].objectId.idx;
                    allcollisions->projectileShip[allcollisions->nrProjectileShip].nrObj1 = nrObj1;
                    allcollisions->projectileShip[allcollisions->nrProjectileShip].object2 = object2[nrObj2].objectId.idx;
                    allcollisions->projectileShip[allcollisions->nrProjectileShip++].nrObj2 = nrObj2;
                    break;
                //enviroment enviroment
                case enviromentEnviroment:
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment].object1 = object1[nrObj1].objectId.idx;
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment].nrObj1 = nrObj1;
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment].object2 = object2[nrObj2].objectId.idx;
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment++].nrObj2 = nrObj2;
                    break;

                }
            }
}

//checks for collisions based on object type
//Sebastian 2015-05-08
void up_checkCollision(struct up_allCollisions *allcollisions){

    int i, j, totalShips = 0, totalObjects = 0, totalProjectiles;
    float distance=0, x=0, y=0, z=0;

    allcollisions->nrProjectileEnviroment = 0;
    allcollisions->nrProjectileShip = 0;
    allcollisions->nrShipEnviroment = 0;
    allcollisions->nrEnviromentEnviroment = 0;
    allcollisions->nrShipShip = 0;

    //list items based on type
    struct up_objectInfo *ships = up_unit_getAllObj(up_ship_type,&totalShips);
    struct up_objectInfo *enviroment = up_unit_getAllObj(up_environment_type, &totalObjects);
    struct up_objectInfo *projectile = up_unit_getAllObj(up_projectile_type, &totalProjectiles);

    //checks ships vs enviroment
    for(i=0; i < totalShips; i++){
        for(j=0; j < totalObjects; j++){
            x = ships[i].pos.x - enviroment[j].pos.x;
            y = ships[i].pos.y - enviroment[j].pos.y;
            z = ships[i].pos.z - enviroment[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <30){
                testCollision(ships, enviroment, i, j, allcollisions, shipEnviroment);
            }
        }
    }
    //projectile vs enviroment
    for(i=0; i < totalProjectiles; i++){
        for(j=0; j < totalObjects; j++){
            x = projectile[i].pos.x - enviroment[j].pos.x;
            y = projectile[i].pos.y - enviroment[j].pos.y;
            z = projectile[i].pos.z - enviroment[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <30){
                testCollision(projectile, enviroment, i, j, allcollisions, projectileEnviroment);
            }
        }
    }
    //ship vs projectile
    for(i=0; i < totalShips; i++){
        for(j=0; j < totalProjectiles; j++){
                if(ships[i].objectId.idx != projectile[j].owner){
                    x = ships[i].pos.x - projectile[j].pos.x;
                    y = ships[i].pos.y - projectile[j].pos.y;
                    z = ships[i].pos.z - projectile[j].pos.z;
                    distance = sqrt((x*x)+(y*y)+(z*z));

                    if(distance <2){
                         testCollision(projectile, ships, j, i, allcollisions, projectileShip);
                    }
                }
        }
    }
    /*
    //enviroment vs enviroment
    for(i=0; i < totalObjects; i++){
        for(j=0; j < totalObjects; j++){
            if(i != j){
                    x = projectile[i].pos.x - projectile[j].pos.x;
                    y = projectile[i].pos.y - projectile[j].pos.y;
                    z = projectile[i].pos.z - projectile[j].pos.z;
                    distance = sqrt((x*x)+(y*y)+(z*z));

                    if(distance <2){
                         testCollision(projectile, ships, j, i, allcollisions, projectileShip);
                    }
            }
        }
    }
    */

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
       // printf("FPS: %d , diffTick: %f globalTickRate: %f\n",up_gFramePerSeconde,diffTick,up_gFrameTickRate);
        fps_counter = 0;

    }

    fps_counter++;
    lastTick = SDL_GetTicks();
}

//this funktion updates the global position of all objects in the world
//only called in the main gameloop once
//Magnus 2015-05-05
void up_updateMovements()
{
    int numObjects = 0;
    struct up_objectInfo *objlocal = NULL;
    float deltaTime = (float)up_getFrameTimeDelta();
    struct up_objectInfo *objectArray = up_unit_getAllObj(up_ship_type,&numObjects);
    int i = 0;
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];

        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_projectile_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];

        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_environment_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];

        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_others_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];

        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }


}
//not in use. moves the playership for testing purposes
//Sebastian
//revised by magnus
void up_updateShipMovment(struct up_objectInfo *ship)
{

    ship->dir.x = sinf(ship->angle);
    ship->dir.y = cosf(ship->angle);
    ship->dir.z = 0;

    //set the max speed forward/backwards
    float sign = (ship->speed < 0 ) ? -1 : 1;
    float speedMagnitude = (ship->speed * sign);
    if (speedMagnitude > 40.0f ) {
        ship->speed = sign * 40.0f;
    }
    // min speed
    if ((1.0f > speedMagnitude) && (speedMagnitude > 0.0f))
    {
        ship->speed = sign * 1.0f;
    }

    float deltaTime = (float)up_getFrameTimeDelta();
    ship->pos.x += ship->dir.x * ship->speed * deltaTime;
    ship->pos.y += ship->dir.y * ship->speed * deltaTime;
    ship->pos.z += ship->dir.z * ship->speed * deltaTime;
}
//not in use. moves the playership for testing purposes
//Sebastian
void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship)
{

    double frameDelta=up_getFrameTimeDelta();
    model->pos.x = ship->pos.x;
    model->pos.y = ship->pos.y;
    model->pos.z = ship->pos.z;

    model->rot.x = 0;
    model->rot.y += 1.0f * frameDelta;
    model->rot.z = ship->angle;

//    model->scale.x = 1;
//    model->scale.y = 1;
//    model->scale.z = 1;

    up_matrixModel(matrixModel,&model->pos, &model->rot, &model->scale);
}

//Tobias 2015-05-05
//magnus 2015-05-06
// magnus 2015-05-21 bug fix
void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent)
{
    char *lineReader = NULL;
    char ammoName[NAMESIZE]="\0";
    char *newLineFinder = "\n";
    char *textRead;
    int tmp1 = 0,tmp2 = 0;

    struct UP_textHandler cdText = up_loadWeaponStatsFile("CoolDown.weapon");

    if(cdText.text == NULL)
    {
        UP_ERROR_MSG("Failed to open the cooldown file.");
        return;
    }
    textRead = cdText.text;


    do
    {
        lineReader = up_token_parser(textRead,&textRead,newLineFinder,strlen(newLineFinder));
        printf("\n");
        if (lineReader == NULL) {
            printf("File read wepeon");
            break;
        }
        if (lineReader[0] == '#') {
            continue;
        }

        if(*lineReader==':')
        {
            printf("found :\n");
            lineReader++;
            sscanf(lineReader,"%d/%d/%s",&tmp1,&tmp2,ammoName);
        }

        printf("%d %d %s",tmp1,tmp2,ammoName);

        if(strcmp(ammoName,"bullet")==0)
        {
            currentEvent->flags.bulletFlag.coolDown = tmp1;
            currentEvent->flags.bulletFlag.amunitionSpeed = tmp2;
        }

        else if(strcmp(ammoName,"missile")==0)
        {
            currentEvent->flags.missileFlag.coolDown = tmp1;
            currentEvent->flags.missileFlag.amunitionSpeed = tmp2;
        }

        else if(strcmp(ammoName,"lazer")==0)
        {
            currentEvent->flags.laserFlag.coolDown = tmp1;
            currentEvent->flags.laserFlag.amunitionSpeed = tmp2;
        }

    }while(textRead <= cdText.text + cdText.length - 1);

    up_textHandler_free(&cdText);
}


/*
struct up_actionState
{
    enum shootingStates fireWeapon;
    enum movement engineState;
    enum turning maneuver;
    int objectID;
};


*/
//struct up_objectInfo *up_unit_objAtIndex(int index);

/*determine the new direction and speed of the object*/
//turnspeed is a set value atm. It is to be stored for each obj
//Sebastian 2015-05-05
//Magnus 2015-05-06
void up_moveObj(struct up_objectInfo *localObject, struct up_actionState *obj, double frameDelta)
{
    //float turnSpeed=1; //temporary. will be unique for each model

    if(obj->engine.state == fwd){
        localObject->speed +=localObject->acceleration*frameDelta;
    }

    if(obj->engine.state==bwd){
        localObject->speed -=localObject->acceleration*frameDelta;
    }

    if(obj->maneuver.state == left){
        //Determines where the object is facing
        localObject->angle = localObject->angle + localObject->turnSpeed*frameDelta;
        localObject->dir.x = sinf(localObject->angle);
        localObject->dir.y = cosf(localObject->angle);

    }

    if(obj->maneuver.state == right){
        //Determines where the object is facing
        localObject->angle = localObject->angle - localObject->turnSpeed*frameDelta;
        localObject->dir.x = sinf(localObject->angle);
        localObject->dir.y = cosf(localObject->angle);

    }

    if(obj->maneuver.state == bankLeft){
        //Determines where the object is facing
        localObject->bankAngle += localObject->turnSpeed*frameDelta;

    }

    if(obj->maneuver.state == bankRight){
        //Determines where the object is facing
        localObject->bankAngle -= localObject->turnSpeed*frameDelta;
    }
}
/*Creates the fired projectiles adding the speed and direction of the ship that fired them*/
//Sebastian 2015-05-05
void up_createProjectile(struct up_objectInfo *localobject,
                         struct up_actionState *obj, struct up_eventState *ammoStats,
                         struct soundLib *sound)
{
    struct up_objectInfo projectile = *localobject;

    //bullet
    if(obj->fireWeapon.state == fireBullet){
        projectile = up_asset_createObjFromId(4);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 100;
        projectile.owner = localobject->objectId.idx;

        up_unit_add(up_projectile_type, projectile);
        obj->fireWeapon.none = none;
    }
    //lazer
    if(obj->fireWeapon.state == fireLaser){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 100;
        projectile.owner = localobject->objectId.idx;

        up_unit_add(up_projectile_type,projectile);
        obj->fireWeapon.none = none;

        //pew pew sound
        up_music(1, 0, sound);

    }
    //missle
    if(obj->fireWeapon.state == fireMissile){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 40;
        projectile.owner = localobject->objectId.idx;

        up_unit_add(up_projectile_type,projectile);
        obj->fireWeapon.none = none;

    }
}

/*updates all action changes in the game*/
//Sebastian 2015-05-05
void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server,
                       int nrObj, struct up_eventState *funkarEj, struct soundLib *sound)
{
    int i=0;
    struct up_objectInfo *localObject = NULL;
    struct up_actionState *tmp;
    double frameDelta=up_getFrameTimeDelta();


    //Updates of objects from the server
    for(i=0; i<nrObj; i++)
    {
        tmp=&server[i];
        if (tmp->objectID.idx == 0) {
            continue;
        }
        localObject = up_unit_objAtIndex(tmp->objectID.type,tmp->objectID.idx);
        if (localObject == NULL) {
            printf("up_update_actions localobject  == NULL \n");
            continue;
        }
        up_moveObj(localObject, tmp,frameDelta);
        up_createProjectile(localObject, tmp, funkarEj, sound);
    }


    //local playership update
    localObject = up_unit_objAtIndex(playerShip->objectID.type,playerShip->objectID.idx);
    up_moveObj(localObject, playerShip, frameDelta);
    up_createProjectile(localObject, playerShip,funkarEj, sound);
}
