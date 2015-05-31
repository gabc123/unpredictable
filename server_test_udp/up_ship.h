#ifndef UP_SHIP_H
#define UP_SHIP_H
#include "up_type.h"
#include "up_modelRepresentation.h"
#include "up_healthbar.h"


struct up_collision
{
    int object1;
    int object2;
};


#define UP_COLLISIONS_MAX 200
struct up_allCollisions
{
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
    projectileEnviroment,
    enviromentEnviroment,
    shipShip
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
    unsigned int ammunitionSpeed;
    int ammunition; // max
    int damage;
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


struct up_player_stats; // forward decleration

void up_server_checkCollision(struct up_allCollisions *allcollisions);

int up_server_handleCollision(struct up_allCollisions *allcollisions,struct up_player_stats *player_stats,struct up_shootingFlag *weapons,struct up_objectID *object_movedArray,int max_moved);


int up_server_update_actions(struct up_actionState *serverArray,
                              struct up_player_stats *playerArray,
                              int nrObj, struct up_eventState *ammoStats,
                              struct up_objectID *object_movedArray,int max_moved);

//this funktion updates the global position of all objects in the world
// only called in the main gameloop once
void up_server_updateMovements();

void up_server_validate_actions(struct up_actionState *playerActionArray,
                                struct up_player_stats *playerInventoryArray,
                                struct up_shootingFlag *player_weaponsArray,
                                int maxPlayers);

void up_server_updateFrameTickRate();
double up_getFrameTimeDelta();
unsigned int up_getFrameRate();

void up_server_weaponCoolDown_start_setup(struct up_eventState *currentEvent);
//int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction);


void up_server_update_playerStats(struct up_allCollisions *collision,struct up_player_stats *stats,struct up_shootingFlag *weapons, int playerId);    //health terminator
#endif
