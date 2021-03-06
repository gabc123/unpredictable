#include "up_deprecated_ship.h"
#include "up_error.h"
#include "up_sdl_redirect.h"
#include "up_utilities.h"
#include "up_shader_module.h"
#include "up_camera_module.h"
#include "up_object_handler.h"
#include "up_render_engine.h"
#include "up_filereader.h"
#include "up_assets.h"
#include "up_music.h"
#include <math.h>
#include "up_healthbar.h"
#define UP_NAMESIZE 100

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct up_container{
    int current;
    int full;
};

struct up_player_stats
{
    
    struct up_container bullets;
    struct up_container missile;
    struct up_container laser;
    struct up_container health;
    struct up_container armor;
};

//Sebastian
struct up_collision
{
    int object1;
    int object2;
};
// Structure that contain every hit with every id of the objects
//Sebastian
#define UP_COLLISIONS_MAX 200
struct up_allCollisions
{
    //type of collision
    struct up_collision projectileEnviroment[UP_COLLISIONS_MAX];
    struct up_collision projectileShip[UP_COLLISIONS_MAX];
    struct up_collision shipEnviroment[UP_COLLISIONS_MAX];
    struct up_collision enviromentEnviroment[UP_COLLISIONS_MAX];
    struct up_collision shipShip[UP_COLLISIONS_MAX];
    //current number of objects in each array
    int nrProjectileEnviroment;
    int nrProjectileShip;
    int nrShipEnviroment;
    int nrEnviromentEnviroment;
    int nrShipShip;
};

//Magnus
enum up_none
{
    none = 0
};

union up_shootingStates
{
    enum up_none none;
    enum
    {
        fireMissile = 1,
        fireBullet,
        fireLaser
    }state;
};


union up_movement
{
    enum up_none none;
    enum
    {
        fwd = 1,
        bwd
    }state;
};

union up_turning
{
    enum up_none none;
    enum
    {
        left = 1,
        right,
        bankLeft,
        bankRight
    }state;
};

//Sebastian
enum collisionType
{
    shipEnviroment,
    projectileShip,
    projectileEnviroment,
    enviromentEnviroment,
    shipShip
};
//Tobias
struct up_actionState
{
    union up_shootingStates fireWeapon;
    union up_movement engine;
    union up_turning maneuver;
    struct up_objectID objectID;
};
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



// magnus , up_updateMovements , 5 maj

double up_getFrameTimeDelta();

double up_gFrameTickRate = 0;
unsigned int up_gFramePerSeconde = 0;

int checkFire(struct cooldownTimer weapon);


//not it use
//Sebastian
//Magnus
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship){
    float deltaTime = (float)up_getFrameTimeDelta();
    ship->speed += 1.0f *(movement->up - movement->down) * deltaTime;
    ship->angle += 1.f *(movement->left - movement->right) * deltaTime;
    if(!(movement->up + movement->down)){ship->speed=0;}
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

unsigned int up_getFrameRate()
{
    return up_gFramePerSeconde;
}

double up_getFrameTimeDelta()
{
    return up_gFrameTickRate;
}

//Sebastian 2015-05-15
// magnus bug checks and fixes
void up_handleCollision(struct up_allCollisions *allcollisions,struct up_player_stats *player_stats,struct up_shootingFlag *weapons)
{
    int i=0;
    struct up_objectInfo *object1 = NULL;
    struct up_objectInfo *object2 = NULL;
    int objectIndex1 = 0;
    int objectIndex2 = 0;
    for(i=0; i < allcollisions->nrShipEnviroment; i++){
        objectIndex1 = allcollisions->shipEnviroment[i].object1;
        objectIndex2 = allcollisions->shipEnviroment[i].object2;

        object1 = up_unit_objAtIndex(up_ship_type, objectIndex1);
        object2 = up_unit_objAtIndex(up_environment_type, objectIndex2);
        if (object1 == NULL){
            UP_ERROR_MSG_INT("Ship env1 tried accesing nonexisting object item nr:",objectIndex1);
            //printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_INT("Ship env2 tried accesing nonexisting object itemnr:",objectIndex2);
            //printf("itemnr: %d\n", i);
            continue;
        }

        object2->dir = object1->dir;
        object2->pos.x += 5*object1->dir.x;
        object2->pos.y += 5*object1->dir.y;
        object2->speed = object1->speed*3/4;
        object1->speed = object1->speed/2;
    }

    for(i=0; i < allcollisions->nrProjectileEnviroment; i++){
        objectIndex1 = allcollisions->projectileEnviroment[i].object1;
        objectIndex2 = allcollisions->projectileEnviroment[i].object2;
        object1 = up_unit_objAtIndex(up_projectile_type, objectIndex1);
        object2 = up_unit_objAtIndex(up_environment_type, objectIndex2);
        if (object1 == NULL){
            UP_ERROR_MSG_INT("proj env1 tried accesing nonexisting object itemnr:",objectIndex1);
            //printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_INT("proj env2 tried accesing nonexisting object itemnr:",objectIndex2);
            //printf("itemnr: %d\n", i);
            continue;
        }

        object2->dir = object1->dir;
        object2->pos.x += 5*object1->dir.x;
        object2->pos.y += 5*object1->dir.y;
        object2->speed = object1->speed*3/4;
        object1->speed = object1->speed/2;
        //up_unit_remove(up_projectile_type, object1->objectId.idx);

    }

    for(i=0; i < allcollisions->nrProjectileShip; i++){
        objectIndex1 = allcollisions->projectileShip[i].object1;
        objectIndex2 = allcollisions->projectileShip[i].object2;
        object1 = up_unit_objAtIndex(up_projectile_type, objectIndex1);
        object2 = up_unit_objAtIndex(up_ship_type, objectIndex2);
        if (object1 == NULL){
            UP_ERROR_MSG_INT("proj ship1 tried accesing nonexisting object itemnr:",objectIndex1);
            //printf("itemnr: %d\n", i);
            continue;
        }

        if (object2 ==NULL){
            UP_ERROR_MSG_INT("proj ship2 tried accesing nonexisting object itemnr:",objectIndex2);
            //printf("itemnr: %d\n", i);
            continue;
        }

        //object2->dir = object1->dir;
        object2->pos.x += 5*object1->dir.x;
        object2->pos.y += 5*object1->dir.y;
//        object2->speed = object1->speed*3/4;
//        object1->speed = object1->speed/2;
        //up_unit_remove(up_projectile_type, object1->objectId.idx);

    }

    for(i=0; i < allcollisions->nrEnviromentEnviroment; i++){
        objectIndex1 = allcollisions->enviromentEnviroment[i].object1;
        objectIndex2 = allcollisions->enviromentEnviroment[i].object2;
        object1 = up_unit_objAtIndex(up_environment_type, objectIndex1);
        object2 = up_unit_objAtIndex(up_environment_type, objectIndex2);
        if (object1 == NULL){
            UP_ERROR_MSG_INT("env env1 tried accesing nonexisting object itemnr:",objectIndex1);
            //printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_INT("env env2 tried accesing nonexisting object itemnr:",objectIndex2);
            //printf("itemnr: %d\n", i);
            continue;
        }
        //2 static object on top of eachother result in no collision
        if(object1->speed == 0 && object2->speed == 0)
            continue;


        if(object1->objectId.idx != object2->owner && object2->objectId.idx != object1->owner){
            object2->owner = object1->objectId.idx;
            object2->dir = object1->dir;
            object2->pos.x += 5*object1->dir.x;
            object2->pos.y += 5*object1->dir.y;
            object2->speed = object1->speed*3/4;
            object1->speed = object1->speed/2;
       }
    }


  /*  if (object2 == NULL) {
        return;
    }
*/
    /*
    for(i=0; i < allcollisions->nrShipShip; i++){
        object1 = up_unit_objAtIndex(up_ship_type, allcollisions->shipShip[i].object1);
        object2 = up_unit_objAtIndex(up_ship_type, allcollisions->shipShip[i].object2);
        if (object1 == NULL){
            UP_ERROR_MSG_INT("tried accesing nonexisting object itemnr:",i);
            //printf("itemnr: %d\n", i);
            continue;
        }
        if (object2 ==NULL){
            UP_ERROR_MSG_INT("tried accesing nonexisting object itemnr:",i);
            //printf("itemnr: %d\n", i);
            continue;
        }
        //2 static object on top of eachother result in no collision
        if(object1->speed == 0 && object2->speed == 0)
            continue;

        if(object1->objectId.idx != object2->owner && object2->objectId.idx != object1->owner){
            object2->owner = object1->objectId.idx;
            object2->dir = object1->dir;
            object2->pos.x += 5*object1->dir.x;
            object2->pos.y += 5*object1->dir.y;
            object2->speed = object1->speed*3/4;
            object1->speed = object1->speed/2;
        }
    }*/

    if (object2 == NULL) {
        return;
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
//Sebastian, reworked and cleaned up the function
static void testCollision(struct up_objectInfo *object1, struct up_objectInfo *object2, struct up_allCollisions *allcollisions, int typeCollision)
{

    float distanceX, distanceY, distanceZ;
    //check the distance between the centerpoints of the objects
    distanceX = fabsf(object2->pos.x - object1->pos.x);
    distanceY = fabsf(object2->pos.y - object1->pos.y);
    distanceZ = fabsf(object2->pos.z - object1->pos.z);

    if (object1->objectId.idx == 0 || object2->objectId.idx == 0) {
        return;
    }

    //checks if the collisionboxes collides with the other objects centerpoint
    if(distanceX <= object1->collisionbox.length.x || distanceX <= object2->collisionbox.length.x){
        if(distanceY <= object1->collisionbox.length.y ||distanceY <=object2->collisionbox.length.y){
            if(distanceZ <= object1->collisionbox.length.z || distanceZ <= object2->collisionbox.length.z)
            {
                //stores the id:s of the colliding objects and increase the number of collisions of that type by 1
                switch(typeCollision)
                {
                case shipEnviroment:
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment].object1 = object1->objectId.idx;
                    allcollisions->shipEnviroment[allcollisions->nrShipEnviroment++].object2 = object2->objectId.idx;
//                    printf("shipEnviroment\n");
//                    printf("object1id stored: %d\n", allcollisions->shipEnviroment[allcollisions->nrShipEnviroment-1].object1);
//                    printf("object2id stored: %d\n", allcollisions->shipEnviroment[allcollisions->nrShipEnviroment-1].object2);

                    break;
                //projectile enviroment
                case projectileEnviroment:
                    //printf("projectileEnviroment\n");
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment].object1 = object1->objectId.idx;
                    allcollisions->projectileEnviroment[allcollisions->nrProjectileEnviroment++].object2 = object2->objectId.idx;
                    break;
                //projectile ship
                case projectileShip:
                    //printf("projectileShip\n");
                    allcollisions->projectileShip[allcollisions->nrProjectileShip].object1 = object1->objectId.idx;
                    allcollisions->projectileShip[allcollisions->nrProjectileShip++].object2 = object2->objectId.idx;
                    break;
                //enviroment enviroment
                case enviromentEnviroment:
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment].object1 = object1->objectId.idx;
                    allcollisions->enviromentEnviroment[allcollisions->nrEnviromentEnviroment++].object2 = object2->objectId.idx;
                    break;
                //ship ship
                case shipShip:
                    //printf("shipship\n");
                    allcollisions->shipShip[allcollisions->nrShipShip].object1 = object1->objectId.idx;
                    allcollisions->shipShip[allcollisions->nrShipShip++].object2 = object2->objectId.idx;
                        if (object1->objectId.idx == 0) {
                            printf("test");
                        }
                    break;
                default:
                        printf("error");
                        break;

                }
            }
        }
    }
}

//checks for collisions based on object type
//Sebastian 2015-05-08
void up_checkCollision(struct up_allCollisions *allcollisions)
{
    int i= 1, j= 1, totalShips = 0, totalObjects = 0, totalProjectiles = 0;
    float distance=0, x=0, y=0, z=0;

    //keeps track of total amount of collisions on current frame based on type
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
    for(i=1; i < totalShips; i++){
        // no need to check collision if unit isnt active, aka removed/hidden
        if (!up_unit_isActive(&ships[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrShipEnviroment >= UP_COLLISIONS_MAX) {
                i = totalShips; // breaks the outer loop if too many collisions of this type somehow occured
                break;
            }
            // no need to check collision if unit isnt active, aka removed/hidden
            if (!up_unit_isActive(&enviroment[j])) {
                continue;
            }
            x = ships[i].pos.x - enviroment[j].pos.x;
            y = ships[i].pos.y - enviroment[j].pos.y;
            z = ships[i].pos.z - enviroment[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance < ships[i].maxLength || distance < enviroment[j].maxLength){
              //  printf("distance = %f\nshiplength = %f\nenviroment = %f\n",distance,ships[i].maxLength,enviroment[j].maxLength);
                testCollision(&ships[i], &enviroment[j], allcollisions, shipEnviroment);
            }
        }
    }

    //projectile vs enviroment
    for(i=1; i < totalProjectiles; i++){
        //no need to check collision if unit isnt active
        if (!up_unit_isActive(&projectile[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrProjectileEnviroment >= UP_COLLISIONS_MAX) {
                i = totalProjectiles; // breaks the outer loop
                break;
            }
            //no need to check collision if unit isnt active
            if (!up_unit_isActive(&enviroment[j])) {
                continue;
            }
            x = projectile[i].pos.x - enviroment[j].pos.x;
            y = projectile[i].pos.y - enviroment[j].pos.y;
            z = projectile[i].pos.z - enviroment[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance < projectile[i].maxLength || distance < enviroment[j].maxLength){
                //printf("projectilelength %f, distance %f, enviroment %f\n",projectile[i].maxLength,distance,enviroment[j].maxLength);
                testCollision(&projectile[i], &enviroment[j], allcollisions, projectileEnviroment);
            }
        }
    }

    //projectile vs ship
    for(i=1; i < totalShips; i++){
        //no need to check collision if unit isnt active
        if (!up_unit_isActive(&ships[i])) {
            continue;
        }
        for(j=1; j < totalProjectiles; j++){
            if (allcollisions->nrProjectileShip >= UP_COLLISIONS_MAX) {
                i = totalShips; // breaks the outer loop
                break;
            }
            //no need to check collision if unit isnt active
            if (!up_unit_isActive(&projectile[j])) {
                continue;
            }
            if(ships[i].objectId.idx != projectile[j].owner){
                x = ships[i].pos.x - projectile[j].pos.x;
                y = ships[i].pos.y - projectile[j].pos.y;
                z = ships[i].pos.z - projectile[j].pos.z;
                distance = sqrt((x*x)+(y*y)+(z*z));

                if(distance < projectile[j].maxLength || distance < ships[i].maxLength){
                    //printf("projectilelength %f, distance %f, ship %f\n",projectile[j].maxLength,distance,ships[i].maxLength);
                    testCollision(&projectile[j], &ships[i], allcollisions, projectileShip);
                }
            }
        }
    }


    //enviroment vs enviroment
    for(i=1; i < totalObjects; i++){
        //no need to check collision if unit isnt active
        if (!up_unit_isActive(&enviroment[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrEnviromentEnviroment >= UP_COLLISIONS_MAX) {
                i = totalObjects; // breaks the outer loop
                break;
            }
            //no need to check collision if unit isnt active
            if (!up_unit_isActive(&enviroment[j])) {
                continue;
            }
            if(i != j){
                    x = enviroment[i].pos.x - enviroment[j].pos.x;
                    y = enviroment[i].pos.y - enviroment[j].pos.y;
                    z = enviroment[i].pos.z - enviroment[j].pos.z;
                    distance = sqrt((x*x)+(y*y)+(z*z));

                    if(distance < enviroment[i].maxLength || distance < enviroment[j].maxLength){
                         testCollision(&enviroment[j], &enviroment[i], allcollisions, enviromentEnviroment);
                    }
            }
        }
    }

    //ship vs ship
    for(i=1; i < totalShips; i++)
    {
        //no need to check collision if unit isnt active
        if (!up_unit_isActive(&ships[i]))
        {
            continue;
        }
        for(j=1; j < totalShips; j++)
        {
            if (allcollisions->nrShipShip >= UP_COLLISIONS_MAX) {
                i = totalShips; // breaks the outer loop
                break;
            }
            //no need to check collision if unit isnt active
            if (!up_unit_isActive(&ships[j]))
            {
                continue;
            }
            if( i != j)
            {
                x = ships[i].pos.x - ships[j].pos.x;
                y = ships[i].pos.y - ships[j].pos.y;
                z = ships[i].pos.z - ships[j].pos.z;
                distance = sqrt((x*x)+(y*y)+(z*z));

                if(distance < ships[i].maxLength || distance < ships[j].maxLength){
                    //printf("ship %f, distance %f, ship %f\n",ships[j].maxLength,distance,ships[i].maxLength);
                    testCollision(&ships[j], &ships[i], allcollisions, shipShip);
                }
            }
        }
    }

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
        if (!up_unit_isActive(objlocal)) {
            continue;
        }
        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_projectile_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];
        if (!up_unit_isActive(objlocal)) {
            continue;
        }
        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_environment_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];
        if (!up_unit_isActive(objlocal)) {
            continue;
        }
        objlocal->pos.x += objlocal->dir.x * objlocal->speed * deltaTime;
        objlocal->pos.y += objlocal->dir.y * objlocal->speed * deltaTime;
        objlocal->pos.z += objlocal->dir.z * objlocal->speed * deltaTime;
    }

    objectArray = up_unit_getAllObj(up_others_type,&numObjects);
    for (i = 0; i < numObjects; i++) {
        objlocal = &objectArray[i];
        if (!up_unit_isActive(objlocal)) {
            continue;
        }
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
void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelOrientation *model,struct up_objectInfo *ship)
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
//magnus 2015-05-21 bug fix
void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent)
{
    char *lineReader = NULL;
    char ammoName[UP_NAMESIZE]="\0";
    char *newLineFinder = "\n";
    char *textRead;
    int cooldown = 0,speed = 0,ammo = 0,damage = 0;

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
            sscanf(lineReader,"%d/%d/%d/%d/%s",&cooldown,&speed,&ammo,&damage,ammoName);
        }

        printf("%d %d %d %d %s",cooldown,speed,ammo,damage,ammoName);

        if(strcmp(ammoName,"bullet")==0)
        {
            currentEvent->flags.bulletFlag.coolDown = cooldown;
            currentEvent->flags.bulletFlag.ammunitionSpeed = speed;
            currentEvent->flags.bulletFlag.ammunition = ammo;
            currentEvent->flags.bulletFlag.damage = damage;
        }

        else if(strcmp(ammoName,"missile")==0)
        {
            currentEvent->flags.missileFlag.coolDown = cooldown;
            currentEvent->flags.missileFlag.ammunitionSpeed = speed;
            currentEvent->flags.missileFlag.ammunition = ammo;
            currentEvent->flags.missileFlag.damage = damage;
        }

        else if(strcmp(ammoName,"lazer")==0)
        {
            currentEvent->flags.laserFlag.coolDown = cooldown;
            currentEvent->flags.laserFlag.ammunitionSpeed = speed;
            currentEvent->flags.laserFlag.ammunition = ammo;
            currentEvent->flags.laserFlag.damage = damage;
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
//walid
//Sebastian
void up_moveObj(struct up_objectInfo *localObject, struct up_actionState *obj, double frameDelta)
{
    //float turnSpeed=1; //temporary. will be unique for each model

    if(obj->engine.state == fwd){
        localObject->speed += localObject->acceleration*frameDelta;
        localObject->bankAngle -= localObject->bankAngle * frameDelta;
    }

    if(obj->engine.state==bwd){
        localObject->speed -=localObject->acceleration*frameDelta;
    }

    if(obj->maneuver.state == left){
        //Determines where the object is facing
        localObject->angle = localObject->angle + localObject->turnSpeed*frameDelta;
        localObject->dir.x = sinf(localObject->angle);
        localObject->dir.y = cosf(localObject->angle);
        localObject->bankAngle += localObject->turnSpeed*frameDelta;
        if(localObject->bankAngle > M_PI/3){
            localObject->bankAngle = M_PI/3;
        }

    }

    if(obj->maneuver.state == right){
        //Determines where the object is facing
        localObject->angle = localObject->angle - localObject->turnSpeed*frameDelta;
        localObject->dir.x = sinf(localObject->angle);
        localObject->dir.y = cosf(localObject->angle);
        localObject->bankAngle -= localObject->turnSpeed*frameDelta;
        if(localObject->bankAngle < -M_PI/3){
            localObject->bankAngle = -M_PI/3;
        }
    }

    if(obj->maneuver.state == bankLeft){
        //tilts the ship to the left
        localObject->bankAngle += localObject->turnSpeed * frameDelta;
    }

    if(obj->maneuver.state == bankRight){
        //tilts the ship to the right
        localObject->bankAngle -= localObject->turnSpeed * frameDelta;
    }
}

/*Creates the fired projectiles adding the speed and direction of the ship that fired them*/
//Sebastian 2015-05-05
void up_createProjectile(struct up_objectInfo *localobject,
                         struct up_actionState *obj, struct up_eventState *ammoStats,
                         struct soundLib *sound)
{
    // disabled in the client , runs on server now
    struct up_objectInfo projectile = {0};
    //checks whether its ok to fire
    struct cooldownTimer *bullet = &ammoStats->flags.bulletFlag;
    struct cooldownTimer *missile = &ammoStats->flags.missileFlag;
    struct cooldownTimer *laser = &ammoStats->flags.laserFlag;

    //bullet
    if(obj->fireWeapon.state == fireBullet){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + bullet->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireBullet;

        bullet->ammunition--;
        //up_unit_add(up_projectile_type, projectile); // disabled in the client , runs on server now

        obj->fireWeapon.none = none;
    }
    //lazer
    if(obj->fireWeapon.state == fireLaser){
        projectile = up_asset_createObjFromId(15);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + laser->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireLaser;
        laser->ammunition--;
        //up_unit_add(up_projectile_type,projectile); // disabled in the client , runs on server now

        obj->fireWeapon.none = none;

        //pew pew sound
        up_music(1, 0, sound);

    }
    //missle
    if(obj->fireWeapon.state == fireMissile){
        projectile = up_asset_createObjFromId(4);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + missile->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireMissile;
        missile->ammunition--;
        //up_unit_add(up_projectile_type,projectile);// disabled in the client , runs on server now

        obj->fireWeapon.none = none;

    }
}

/*updates all action changes in the game*/
//Sebastian 2015-05-05
void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server,
                       int nrObj, struct up_eventState *ammoStats, struct soundLib *sound)
{
    int i=0;
    struct up_objectInfo *localObject = NULL;
    struct up_actionState *tmp;
    double frameDelta=up_getFrameTimeDelta();

    //Updates of objects from the server
    //Also handles the creation of projectiles
    for(i=0; i<nrObj; i++)
    {
        tmp=&server[i];
        if (tmp->objectID.idx == 0) {
            continue;
        }
        localObject = up_unit_objAtIndex(tmp->objectID.type,tmp->objectID.idx);
        if (localObject == NULL) {
            printf("PLayer disconnected, removed \n");
            server[i].objectID.idx = 0;
            continue;
        }
        up_moveObj(localObject, tmp,frameDelta);
        up_createProjectile(localObject, tmp, ammoStats, sound);
    }

    //local playership update
    localObject = up_unit_objAtIndex(playerShip->objectID.type,playerShip->objectID.idx);
    if (localObject == NULL) {
        // the ship has been destroyed
        return ;
    }
    up_moveObj(localObject, playerShip, frameDelta);
    up_createProjectile(localObject, playerShip,ammoStats, sound);
}

// moved to the server form here and down
//walled
//static void take_powerUp(struct up_player_stats *stats,int damage){
//
//
//    stats->health.current += damage;
//
//    if(stats->health.current >= stats->health.full){
//        stats->armor.current += stats->health.current - stats->health.full;
//        stats->health.current = stats->health.full;
//    }
//
//    if(stats->armor.current >= stats->armor.full){
//        stats->armor.current = stats->armor.full;
//    }
//
//}
//walled
//static void take_damage(struct up_player_stats *stats,int damage){
//
//    stats->armor.current -= damage;
//    if(stats->armor.current < 0){
//        stats->health.current += stats->armor.current;
//        stats->armor.current = 0;
//
//    }
//
//    stats->health.current = (stats->health.current > 0) ? stats->health.current : 0;
//
//}

/*
 fireMissile = 1,
 fireBullet,
 fireLaser


 */

 //Wallid + Magnus
//static void ship_projectileHit(struct up_player_stats *player,struct up_shootingFlag *weapons,struct up_objectInfo *projectile)
//{
//    int damage = 0;
//    switch (projectile->projectile) {
//        case fireMissile:
//            damage = weapons->missileFlag.damage;
//            break;
//        case fireBullet:
//            damage = weapons->bulletFlag.damage;
//            break;
//        case fireLaser:
//            damage = weapons->laserFlag.damage;
//            break;
//
//        default:
//            damage = 0;
//            break;
//    }
//    take_damage(player,damage);
//
//}
//


//walled
// magnus included weapons so everything gets synced to the same state
void up_update_playerStats(struct up_allCollisions *collision,struct up_player_stats *player,struct up_shootingFlag *weapons, int playerId)                         //"Den checkar :P "
{

    // disabled on client;
    /*
    int i=0;
    int other_shipId;
    struct up_objectInfo *other_object = NULL;
    struct up_objectInfo *player_object = NULL;

    other_shipId = collision->shipShip[i].object2;
    player_object = up_unit_objAtIndex(up_ship_type, playerId);



    for(i=0; i<collision->nrShipEnviroment; i++){
        other_shipId = collision->shipShip[i].object2;
        other_object = up_unit_objAtIndex(up_environment_type, other_shipId);

        if (other_object == NULL) {
            continue;
        }
        if(collision->shipEnviroment[i].object1 == playerId){

            take_damage(player,7);// disabled in the client , runs on server now


        }
    }

    for(i=0; i<collision->nrShipShip; i++){

        if(collision->shipShip[i].object1 == playerId){
            other_shipId = collision->shipShip[i].object2;
            other_object = up_unit_objAtIndex(up_ship_type, other_shipId);

            if (other_object == NULL) {
                continue;
            }

            if(other_shipId !=  playerId ||
               other_object->modelId == player_object->modelId){
                take_damage(player,5);// disabled in the client , runs on server now

            }
        }
    }


    for(i=0; i<collision->nrProjectileShip; i++){
        if(collision->projectileShip[i].object2 == playerId){
            other_shipId = collision->projectileShip[i].object1;
            other_object = up_unit_objAtIndex(up_projectile_type, other_shipId);

            if (other_object == NULL) {
                continue;
            }
            if(other_object->modelId == 7){
                take_powerUp(player,20);
            }
            if (other_object->modelId ) {
                ship_projectileHit(player,weapons,other_object);
            }
        }
    }
*/
    // still in use by the client 
    player->bullets.current = weapons->bulletFlag.ammunition;
    player->missile.current = weapons->missileFlag.ammunition;
    player->laser.current = weapons->laserFlag.ammunition;
}







