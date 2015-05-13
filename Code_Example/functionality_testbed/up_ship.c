#include "up_ship.h"
#include "up_error.h"
#include "up_sdl_redirect.h"
#include "up_utilities.h"
#include "up_shader_module.h"
#include "up_camera_module.h"
#include "up_modelRepresentation.h"
#include "up_filereader.h"
#include "up_assets.h"
#include "up_music.h"
#define NAMESIZE 100


// magnus , up_updateMovements , 5 maj

double up_getFrameTimeDelta();

double up_gFrameTickRate = 0;
unsigned int up_gFramePerSeconde = 0;

int checkFire(unsigned int startTime, unsigned int cooldown);


//not it use
//Sebastian
//Revised by Magnus
void shipMove(struct shipMovement *movement, struct up_objectInfo *ship){
    float deltaTime = (float)up_getFrameTimeDelta();
    ship->speed += 1.0f *(movement->up - movement->down) * deltaTime;
    ship->angle += 1.f *(movement->left - movement->right) * deltaTime;
    if(!(movement->up + movement->down)){ship->speed=0;}
}




int UP_eventHandler(struct up_eventState *currentEvent, struct up_actionState *objectAction)
{
    int flag = 1;
    SDL_Event event;
    int tempFlag = 0;


    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flag = 0;
        }
        if(event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    objectAction->engine.state = fwd;
                    break;
                case SDLK_s:
                    objectAction->engine.state = bwd;
                    break;
                case SDLK_a:
                    objectAction->maneuver.state = left;
                    break;
                case SDLK_d:
                    objectAction->maneuver.state = right;
                    break;
                //Rotate right
                case SDLK_e:
                    objectAction->maneuver.state = bankRight;
                    break;
                case SDLK_q:
                    objectAction->maneuver.state = bankLeft;
                    break;



                case SDLK_r:
                    up_cam_zoom(1.0f);
                    break;
                case SDLK_f:
                    up_cam_zoom(-1.0f);
                    break;

                //fire main weapon for playership
                case SDLK_SPACE:
                    //Checks if its ok to fire a projectile
                    tempFlag = checkFire(currentEvent->flags.bulletFlag.startTime, currentEvent->flags.bulletFlag.coolDown);

                    if(tempFlag==0)
                    {
                        objectAction->fireWeapon.state = fireBullet;
                        currentEvent->flags.bulletFlag.startTime = SDL_GetTicks();
                    }else
                    {
                        objectAction->fireWeapon.state = none;
                    }
                    break;
                //missle
                case SDLK_c:
                    tempFlag = checkFire(currentEvent->flags.missileFlag.startTime, currentEvent->flags.missileFlag.coolDown);

                    if(tempFlag==0)
                    {
                        objectAction->fireWeapon.state = fireMissile;
                        currentEvent->flags.missileFlag.startTime = SDL_GetTicks();
                    }else
                    {
                        objectAction->fireWeapon.state = none;
                    }
                    break;
                //lazer
                case SDLK_v:
                    tempFlag = checkFire(currentEvent->flags.laserFlag.startTime, currentEvent->flags.laserFlag.coolDown);

                    if(tempFlag==0)
                    {
                        objectAction->fireWeapon.state = fireLaser;
                        currentEvent->flags.laserFlag.startTime = SDL_GetTicks();
                    }else
                    {
                        objectAction->fireWeapon.state = none;
                    }
                    break;
                default:
                    break;
            }
        }

        if(event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    objectAction->engine.none=none;
                    break;
                case SDLK_s:
                    objectAction->engine.none=none;
                    break;
                case SDLK_d:
                    objectAction->maneuver.none=none;
                    break;
                case SDLK_a:
                    objectAction->maneuver.none=none;
                    break;
                case SDLK_SPACE:
                    objectAction->fireWeapon.none=none;
                    break;
                case SDLK_e:
                    objectAction->maneuver.none = none;
                    break;
                case SDLK_q:
                    objectAction->maneuver.none = none;
                    break;
                default:
                    break;
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

//Sebastian + Tobias 2015-05-12
//checks objects collisionboxes for whether a hit has occured or not
void testCollision(struct up_objectInfo *enviroment, struct up_objectInfo *ship, int i, int j)
{
    struct Hitbox hitShip = {ship[i].pos.x+3.0, ship[i].pos.y+3.0, ship[i].pos.z+3.0,  ship[i].pos.x-3.0,  ship[i].pos.y-3.0,  ship[i].pos.z-3.0};
    struct Hitbox otherModel ={enviroment[j].pos.x+1.0, enviroment[j].pos.y+1.0, enviroment[j].pos.z+5.0,  enviroment[j].pos.x-1.0,  enviroment[j].pos.y-1.0,  enviroment[j].pos.z-5.0};
    if((hitShip.xmax < otherModel.xmax && hitShip.xmax > otherModel.xmin) || (hitShip.xmin > otherModel.xmin && hitShip.xmin < otherModel.xmax))
        if((hitShip.ymax < otherModel.ymax && hitShip.ymax > otherModel.ymin) || (hitShip.ymin > otherModel.ymin && hitShip.ymin < otherModel.ymax))
            if((hitShip.zmax < otherModel.zmax && hitShip.zmax > otherModel.zmin) || (hitShip.zmin > otherModel.zmin && hitShip.zmin < otherModel.zmax))
            {
                printf("ship %d collision with enviroment id %d\n", i, j);
                enviroment[j].dir=ship[i].dir;
                enviroment[j].pos.x+=5*ship[i].dir.x;
                enviroment[j].pos.y+=5*ship[i].dir.y;
                enviroment[j].pos.z+=5*ship[i].dir.z;
                enviroment[j].speed=ship[i].speed*3/4;
                ship[i].speed =ship[i].speed/2;
            }


}

//checks for collisions based on object type
//Sebastian
void up_checkCollision(){

    int i, j, totalShips = 0, totalObject = 0;
    float distance=0, x=0, y=0, z=0;

    struct up_objectInfo *ships = up_unit_getAllObj(up_ship_type,&totalShips);
    struct up_objectInfo *enviroment = up_unit_getAllObj(up_environment_type, &totalObject);
    for(i=0; i < totalShips; i++){
        for(j=0; j < totalObject; j++){
            x = ships[i].pos.x - enviroment[j].pos.x;
            y = ships[i].pos.y - enviroment[j].pos.y;
            z = ships[i].pos.z - enviroment[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <30){
                testCollision(enviroment,ships, i, j);

            }
        }
    }

    struct up_objectInfo *projectile = up_unit_getAllObj(up_projectile_type, &totalObject);
    for(i=0; i < totalShips; i++){
        for(j=0; j < totalObject; j++){
            x = ships[i].pos.x - projectile[j].pos.x;
            y = ships[i].pos.y - projectile[j].pos.y;
            z = ships[i].pos.z - projectile[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <2){
                 testCollision(projectile,ships, i, j);
            }
        }
    }

    /*struct up_objectInfo *others = up_unit_getAllObj(up_others_type, &totalObject);
    for(i=0; i < totalShips; i++){
        for(j=0; j < totalObject; j++){
            x = ships[i].pos.x - others[j].pos.x;
            y = ships[i].pos.y - others[j].pos.y;
            z = ships[i].pos.z - others[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <2){
                printf("ship %d collision\n", i);
            }
        }
    }
    */

    /*for(i=0; i < totalShips; i++){
        for(j=i+1; j < totalShips-1; j++){
            x = ships[i].pos.x - ships[j].pos.x;
            y = ships[i].pos.y - ships[j].pos.y;
            z = ships[i].pos.z - ships[j].pos.z;
            distance = sqrt((x*x)+(y*y)+(z*z));

            if(distance <2){
                printf("ship %d collision\n", i);
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
        if(*lineReader==':')
        {
            printf("found :\n");
            lineReader++;
            sscanf(lineReader,"%d %d %s",&tmp1,&tmp2,ammoName);
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
    if(obj->fireWeapon.state==fireBullet){
        projectile = up_asset_createObjFromId(4);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 100;
       // projectile.pos.x += 2;


        up_unit_add(up_projectile_type,projectile);
        obj->fireWeapon.none = none;

    }
    //lazer
    if(obj->fireWeapon.state==fireLaser){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 100;
        up_unit_add(up_projectile_type,projectile);
        obj->fireWeapon.none = none;
    
        //pew pew sound
        up_music(1, 0, sound);

    }
    //missle
    if(obj->fireWeapon.state==fireMissile){
        projectile = up_asset_createObjFromId(0);
        projectile.pos = localobject->pos;
        projectile.dir = localobject->dir;
        projectile.angle = localobject->angle;
        projectile.speed = localobject->speed + 40;
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
            return;
        }
        up_moveObj(localObject, tmp,frameDelta);
        up_createProjectile(localObject, tmp, funkarEj, sound);
    }


    //local playership update
    localObject = up_unit_objAtIndex(playerShip->objectID.type,playerShip->objectID.idx);
    up_moveObj(localObject, playerShip, frameDelta);
    up_createProjectile(localObject, playerShip,funkarEj, sound);
}
