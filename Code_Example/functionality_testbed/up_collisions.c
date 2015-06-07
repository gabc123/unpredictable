//
//  up_collisions.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_collisions.h"

#include "up_error.h"

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