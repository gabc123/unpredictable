#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_type.h"
#include "up_matrixTransforms.h"
#include "up_modelRepresentation.h"
#include "up_music.h"
#include "up_healthbar.h"

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

struct up_key_map *up_key_remapping_setup();


struct up_player_stats; // forward declaration

//Stores occuring collisions in parameter allcollisions
void up_checkCollision(struct up_allCollisions *allcollisions);
//Handles stored collisions based on the type of the colliding objects
void up_handleCollision(struct up_allCollisions *allcollisions,struct up_player_stats *player_stats,struct up_shootingFlag *weapons);
//Not in use. Handled playership movement
void up_updatShipMatrixModel(up_matrix4_t *matrixModel,struct up_modelRepresentation *model,struct up_objectInfo *ship);
//updates all actionchanges in the game
void up_update_actions(struct up_actionState *playerShip, struct up_actionState *server, int nrObj, struct up_eventState *smmostats, struct soundLib *sound);

//this funktion updates the global position of all objects in the world
//Only called in the main gameloop once
void up_updateMovements();
//no longer in use. Made for testing purposes
void up_updateShipMovment(struct up_objectInfo *ship);

//A function that ensures values are adapted to correct
void up_updateFrameTickRate();
//Returns the time between current and former frame
double up_getFrameTimeDelta();
//Frames per second
unsigned int up_getFrameRate();

void up_weaponCoolDown_start_setup(struct up_eventState *currentEvent);
//handles keyinputs
int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction,struct up_key_map *up_keymap);
//not in use. Temporary used for testing keyinputs during development
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship);
//keeps track and handles health and ammo
void up_update_playerStats(struct up_allCollisions *collision,struct up_player_stats *stats,struct up_shootingFlag *weapons, int playerId);    //health terminator
#endif
