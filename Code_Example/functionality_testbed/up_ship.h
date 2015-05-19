#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_type.h"
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"
#include "up_music.h"

struct up_collision
{
    int object1[100];
    int object2[100];
    int nrCollisions;
};

struct up_allCollisions
{
    struct up_collision projectileEnviroment;
    struct up_collision projectileShip;
    struct up_collision shipEnviroment;
};

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

enum collisionType
{
    shipEnviroment,
    projectileShip,
    projectileEnviroment
};
struct up_actionState
{
    union up_shootingStates fireWeapon;
    union up_movement engine;
    union up_turning maneuver;
    struct up_objectID objectID;
};

struct cooldownTimer
{
    unsigned int startTime;
    unsigned int coolDown;
    unsigned int amunitionSpeed;
};

struct up_shootingFlag
{

    struct cooldownTimer bulletFlag;
    struct cooldownTimer missileFlag;
    struct cooldownTimer laserFlag;
};

struct up_eventState
{
    struct up_shootingFlag flags;
};

struct up_key_map
{
    char name[25];
    SDL_Keycode key;
    void (*keyDown_func)(struct up_eventState *currentEvent,struct up_actionState *objectAction);
    void (*keyUp_func)(struct up_eventState *currentEvent,struct up_actionState *objectAction);
};

struct up_key_map *up_key_remapping_setup();

void up_checkCollision(struct up_allCollisions *allcollisions);
void up_handleCollision(struct up_allCollisions *allcollisions);
void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);

void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server, int nrObj, struct up_eventState *funkarEj, struct soundLib *sound);

//this funktion updates the global position of all objects in the world
// only called in the main gameloop once
void up_updateMovements();

void up_updateShipMovment(struct up_objectInfo *ship);

void up_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();

void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent);
int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction,struct up_key_map *up_keymap);

void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
#endif
