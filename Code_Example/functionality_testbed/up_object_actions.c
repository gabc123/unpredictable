//
//  up_object_actions.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_object_actions.h"
#include "up_game_tools.h"
#include "up_object_handler.h"
#include "up_control_events.h"
#include "up_assets.h"
#include "up_music.h"
#include <math.h>


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
