#include "up_ship.h"
#include "up_error.h"
#include "up_utilities.h"
#include "up_modelRepresentation.h"
#include "up_filereader.h"
#include "up_assets.h"
#include <math.h>
#include "up_healthbar.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NAMESIZE 100


// magnus , up_updateMovements , 5 maj

double up_getFrameTimeDelta();







int up_server_projectile_reaping(struct up_objectID *object_movedArray,int max_moved)
{
    int objectMoved_count = 0;
    int numberObj = 0;
    struct up_objectInfo *projectilArray = up_unit_getAllObj(up_projectile_type, &numberObj);
    int i = 0;
    unsigned int currentTime = up_clock_ms();
    for (i = 0; i < max_moved; i++) {
        if (!up_unit_isActive(&projectilArray[i])) {
            continue;
        }
        
        // after 10 seconds the projectile gets removed
        if ((projectilArray[i].spawnTime - currentTime < 10000) ||
            (projectilArray[i].spawnTime != 0)) //powerups
        {
            continue;
        }
        
        if(objectMoved_count < max_moved){
            object_movedArray[objectMoved_count] = projectilArray[i].objectId;
            objectMoved_count++;
            up_unit_remove(projectilArray[i].objectId.type, projectilArray[i].objectId.idx);
        }
    }
    return objectMoved_count;
}


//Sebastian 2015-05-15
// magnus bug checks and fixes
// magnus server version
int up_server_handleCollision(struct up_allCollisions *allcollisions,struct up_player_stats *player_stats,struct up_shootingFlag *weapons,struct up_objectID *object_movedArray,int max_moved)
{
    int i=0;
    struct up_objectInfo *object1 = NULL;
    struct up_objectInfo *object2 = NULL;
    int objectIndex1 = 0;
    int objectIndex2 = 0;
    
    int objectMoved_count = 0;  // this keeps track of number of objects that needs to be updated.
    
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
        
        if (objectMoved_count < max_moved) {
            object_movedArray[objectMoved_count] = object1->objectId;   // so we know what to update
            objectMoved_count++;
            object_movedArray[objectMoved_count] = object2->objectId;   // so we know what to update
            objectMoved_count++;
        }
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
        
        if (objectMoved_count < max_moved) {
            object_movedArray[objectMoved_count] = object1->objectId;   // so we know what to update
            objectMoved_count++;
            object_movedArray[objectMoved_count] = object2->objectId;   // so we know what to update
            objectMoved_count++;
        }
        up_unit_remove(up_projectile_type, object1->objectId.idx);
        
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
        
        if (objectMoved_count < max_moved) {
            object_movedArray[objectMoved_count] = object1->objectId;   // so we know what to update
            objectMoved_count++;
            object_movedArray[objectMoved_count] = object2->objectId;   // so we know what to update
            objectMoved_count++;
        }
        up_unit_remove(up_projectile_type, object1->objectId.idx);
        
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
            if (objectMoved_count < max_moved) {
                object_movedArray[objectMoved_count] = object1->objectId;   // so we know what to update
                objectMoved_count++;
                object_movedArray[objectMoved_count] = object2->objectId;   // so we know what to update
                objectMoved_count++;
            }
            
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
    

    return objectMoved_count;
}

//Sebastian + Tobias 2015-05-12
//checks objects collisionboxes too see whether a hit has occured or not
static void testCollision(struct up_objectInfo *object1, struct up_objectInfo *object2, struct up_allCollisions *allcollisions, int typeCollision)
{
    
    float distanceX, distanceY, distanceZ;
    //check the distance between the centerpoints of the objects
    distanceX = fabsf(object2->pos.x - object1->pos.x);
    distanceY = fabsf(object2->pos.y - object1->pos.y);
    distanceZ = fabsf(object2->pos.z - object1->pos.z);
    
    if (object1->objectId.idx == 0|| object2->objectId.idx == 0) {
        return;
    }
    
    //checks if the collisionboxes collides
    if(distanceX <= object1->collisionbox.length.x || distanceX <= object2->collisionbox.length.x){
        if(distanceY <= object1->collisionbox.length.y ||distanceY <=object2->collisionbox.length.y){
            if(distanceZ <= object1->collisionbox.length.z || distanceZ <= object2->collisionbox.length.z)
            {
                //stores the id, arrayplacement by the type of collision
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
void up_server_checkCollision(struct up_allCollisions *allcollisions){
    
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
        // no need to check ship if not active
        if (!up_unit_isActive(&ships[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrShipEnviroment >= UP_COLLISIONS_MAX) {
                i = totalShips; // breaks the outer loop
                break;
            }
            // no need to check ship if not active
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
        // no need to check ship if not active
        if (!up_unit_isActive(&projectile[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrProjectileEnviroment >= UP_COLLISIONS_MAX) {
                i = totalProjectiles; // breaks the outer loop
                break;
            }
            // no need to check ship if not active
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
        // no need to check ship if not active
        if (!up_unit_isActive(&ships[i])) {
            continue;
        }
        for(j=1; j < totalProjectiles; j++){
            if (allcollisions->nrProjectileShip >= UP_COLLISIONS_MAX) {
                i = totalShips; // breaks the outer loop
                break;
            }
            // no need to check ship if not active
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
        // no need to check ship if not active
        if (!up_unit_isActive(&enviroment[i])) {
            continue;
        }
        for(j=1; j < totalObjects; j++){
            if (allcollisions->nrEnviromentEnviroment >= UP_COLLISIONS_MAX) {
                i = totalObjects; // breaks the outer loop
                break;
            }
            // no need to check ship if not active
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
        // no need to check ship if not active
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
            // no need to check ship if not active
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
                    if (allcollisions->nrEnviromentEnviroment > 5) {
                        printf("test");
                    }
                    testCollision(&ships[j], &ships[i], allcollisions, shipShip);
                }
            }
        }
    }
    
}

double up_gFrameTickRate = 0;

double up_getFrameTimeDelta()
{
    return up_gFrameTickRate;
}

//Magnus
void up_updateFrameTickRate()
{
    static clock_t lastTick = 0;
    
    //how many milliseconds have pased between frames
    
    double diffTick = (clock() - lastTick);
    
    // this fuction overflows every 72 min when clock_t is 32bit
    // this guards against that
    diffTick = (diffTick > 0) ? diffTick : 0;
    
    // this results in fraction of a seconde that passed between frames
    // used to give smooth movment regardless of fps
    up_gFrameTickRate = diffTick/CLOCKS_PER_SEC;
    
    lastTick = clock();
}

//this funktion updates the global position of all objects in the world
//only called in the main gameloop once
//Magnus 2015-05-05
void up_server_updateMovements()
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



//Tobias 2015-05-05
//magnus 2015-05-06
// magnus 2015-05-21 bug fix
void up_server_weaponCoolDown_start_setup(struct up_eventState *currentEvent)
{
    char *lineReader = NULL;
    char ammoName[NAMESIZE]="\0";
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
        //Determines where the object is facing
        localObject->bankAngle += localObject->turnSpeed * frameDelta;
    }
    
    if(obj->maneuver.state == bankRight){
        //Determines where the object is facing
        localObject->bankAngle -= localObject->turnSpeed * frameDelta;
    }
}




//Tobias
// magnus , ammocheck
int checkFire(struct cooldownTimer weapon,struct up_player_stats *playerInventory)
{
    unsigned int tempVar= up_clock_ms() - weapon.startTime;
    
    if (tempVar>= weapon.coolDown && weapon.ammunition > 0)
    {
        return 1;
    }
    return 0;
}


// checks if this is a valid action
static void validate_action(struct up_actionState *playerAction,
                            struct up_player_stats *playerInventory,
                            struct up_shootingFlag *player_weapons)
{
    switch (playerAction->fireWeapon.state) {
        case fireBullet:
            if (playerInventory->bullets.current <= 0) {
                playerAction->fireWeapon.state = none;
            }
            break;
        case fireMissile:
            if (playerInventory->missile.current <= 0) {
                playerAction->fireWeapon.state = none;
            }
            break;
        case fireLaser:
            if (playerInventory->laser.current <= 0) {
                playerAction->fireWeapon.state = none;
            }
            break;
            
        default:
            break;
    }
    
}


void up_server_validate_actions(struct up_actionState *playerActionArray,
                                struct up_player_stats *playerInventoryArray,
                                struct up_shootingFlag *player_weaponsArray,
                                int maxPlayers)
{
    int i = 0;
    for (i = 0; i < maxPlayers; i++) {
        if(playerActionArray[i].objectID.idx == 0)
        {
            continue;
        }
        validate_action(&playerActionArray[i],&playerInventoryArray[i],&player_weaponsArray[i]);
    }
    
    
}


/*Creates the fired projectiles adding the speed and direction of the ship that fired them*/
//Sebastian 2015-05-05
// magnus server modifactions
static int up_server_createProjectile(struct up_objectInfo *localobject,
                                       struct up_actionState *player_action,
                                       struct up_player_stats *player_inventory,
                                       struct up_eventState *ammoStats)
{
    struct up_objectInfo projectile = {0};
    struct cooldownTimer *bullet = &ammoStats->flags.bulletFlag;
    struct cooldownTimer *missile = &ammoStats->flags.missileFlag;
    struct cooldownTimer *laser = &ammoStats->flags.laserFlag;
    int index = 0;
    //bullet
    if(player_action->fireWeapon.state == fireBullet){
        projectile = up_asset_createObjFromId(4);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + bullet->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireBullet;
        projectile.spawnTime = up_clock_ms();
        
        player_inventory->bullets.current--;
        
        index = up_unit_add(up_projectile_type, projectile);
        player_action->fireWeapon.none = none;
    }
    //lazer
    if(player_action->fireWeapon.state == fireLaser){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + laser->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireLaser;
        projectile.spawnTime = up_clock_ms();
        
        player_inventory->laser.current--;
        index = up_unit_add(up_projectile_type,projectile);
        player_action->fireWeapon.none = none;
        
    }
    //missle
    if(player_action->fireWeapon.state == fireMissile){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + missile->ammunitionSpeed;
        projectile.owner = localobject->objectId.idx;
        projectile.projectile = fireMissile;
        projectile.spawnTime = up_clock_ms();
        
        player_inventory->missile.current--;
        index = up_unit_add(up_projectile_type,projectile);
        player_action->fireWeapon.none = none;
        
    }
    return index;
}

/*updates all action changes in the game*/
//Sebastian 2015-05-05
// magnus server mod, object moved array
int up_server_update_actions(struct up_actionState *serverArray,
                              struct up_player_stats *playerArray,
                              int nrObj, struct up_eventState *ammoStats,
                              struct up_objectID *object_movedArray,int max_moved)
{
    int i=0;
    struct up_objectInfo *localObject = NULL;
    struct up_actionState *tmp;
    double frameDelta=up_getFrameTimeDelta();
    
    int objectCreated_count = 0;
    struct up_objectID tmpObjId = {0};
    tmpObjId.type = up_projectile_type;
    
    //Updates of objects from the server
    for(i=0; i<nrObj; i++)
    {
        tmp=&serverArray[i];
        if (tmp->objectID.idx == 0) {
            continue;
        }
        localObject = up_unit_objAtIndex(tmp->objectID.type,tmp->objectID.idx);
        if (localObject == NULL) {
            //printf("up_update_actions localobject  == NULL \n");
            continue;
        }
        playerArray[i].modelId = localObject->modelId;
        up_moveObj(localObject, tmp,frameDelta);
        tmpObjId.idx = up_server_createProjectile(localObject, tmp, &playerArray[i], ammoStats);
        if (objectCreated_count < max_moved) {
            object_movedArray[objectCreated_count] = tmpObjId;  // add the update
            objectCreated_count++;
        }
        
    }
    
    
    return objectCreated_count;
}

//walled
static void take_powerUp(struct up_player_stats *stats,int damage){
    
    
    stats->health.current += damage;
    
    if(stats->health.current >= stats->health.full){
        stats->armor.current += stats->health.current - stats->health.full;
        stats->health.current = stats->health.full;
    }
    
    if(stats->armor.current >= stats->armor.full){
        stats->armor.current = stats->armor.full;
    }
    
}

//walled
static void take_damage(struct up_player_stats *stats,int damage){
    
    stats->armor.current -= damage;
    if(stats->armor.current < 0){
        stats->health.current += stats->armor.current;
        stats->armor.current = 0;
    }
    
    stats->health.current = (stats->health.current > 0) ? stats->health.current : 0;
    
}

/*
 fireMissile = 1,
 fireBullet,
 fireLaser
 
 
 */
static void ship_projectileHit(struct up_player_stats *player,struct up_shootingFlag *weapons,struct up_objectInfo *projectile)
{
    int damage = 0;
    switch (projectile->projectile) {
        case fireMissile:
            damage = weapons->missileFlag.damage;
            break;
        case fireBullet:
            damage = weapons->bulletFlag.damage;
            break;
        case fireLaser:
            damage = weapons->laserFlag.damage;
            break;
            
        default:
            damage = 0;
            break;
    }
    take_damage(player,damage);
    
}

//walled
// magnus included weapons so everything gets synced to the same state
void up_server_update_playerStats(struct up_allCollisions *collision,
                                  struct up_player_stats *player_statsArray,
                                  struct up_shootingFlag *weapons_info,
                                  int maxPlayers)
{
    
    
    int i=0;
    int other_shipId;
    int playerId;
    struct up_objectInfo *other_object = NULL;
    //struct up_objectInfo *player_object = NULL;
    
    for(i=0; i<collision->nrShipEnviroment; i++){
        
        playerId = collision->shipEnviroment[i].object1;
        other_shipId = collision->shipShip[i].object2;
        other_object = up_unit_objAtIndex(up_environment_type, other_shipId);
        
        if (other_object == NULL) {
            continue;
        }
        
        if (playerId < maxPlayers) {
            take_damage(&player_statsArray[playerId],7);
        }
        
    }
    
    for(i=0; i<collision->nrShipShip; i++){
        
        playerId = collision->shipShip[i].object1;
        other_shipId = collision->shipShip[i].object2;
        other_object = up_unit_objAtIndex(up_ship_type, other_shipId);
        
        if (other_object == NULL) {
            continue;
        }
        
        if(other_shipId !=  playerId){
            take_damage(&player_statsArray[playerId],5);
        }
        
    }
    
    for(i=0; i<collision->nrProjectileShip; i++){
        
        playerId = collision->shipShip[i].object2;
        other_shipId = collision->projectileShip[i].object1;
        other_object = up_unit_objAtIndex(up_projectile_type, other_shipId);
        
        if (other_object == NULL) {
            continue;
        }
        if(other_object->modelId == 7){
            take_powerUp(&player_statsArray[playerId],20);
        }
        if (other_object->modelId ) {
            ship_projectileHit(&player_statsArray[playerId],weapons_info,other_object);
        }
        
    }
    
}







