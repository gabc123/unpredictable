#include "up_ship.h"
#include "up_error.h"
#include "up_sdl_redirect.h"
#include "up_shader_module.h"
#include "up_camera_module.h"

double up_getFrameTimeDelta();

double up_gFrameTickRate = 0;
unsigned int up_gFramePerSeconde = 0;

int checkFire(int shootFlag)


void shipMove(struct shipMovement *movement, struct up_objectInfo *ship){
    float deltaTime = (float)up_getFrameTimeDelta();
    ship->speed += 1.0f *(movement->up - movement->down) * deltaTime;
    ship->angle += 1.f *(movement->left - movement->right) * deltaTime;
    if(!(movement->up + movement->down)){ship->speed=0;}
}

struct up_eventState
{
    struct up_shootingFlag flags;
};

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
                    objectAction.engineState = forwrd;
                    break;
                case SDLK_s:
                    objectAction.engineState = backwrd;
                    break;
                case SDLK_a:
                    objectAction.maneuver = left;
                    break;
                case SDLK_d:
                    objectAction.maneuver = right;
                    break;


                case SDLK_r:
                    up_cam_zoom(1.0f);
                    break;
                case SDLK_f:
                    up_cam_zoom(-1.0f);
                    break;


                case SDLK_SPACE:
                    tempFlag = checkFire(currentEvent.flags.bulletFlag);
                    if(tempFlag=0)
                    {
                        objectAction.fireWeapon = fireBullet;
                        currentEvent.flags.bulletFlag.startTime = SDL_GetTicks();
                    }else
                    {
                        objectAction.fireWeapon = none;
                    }
                    break;

                case SDLK_c:


                default:
                    break;
            }
        }

        if(event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    objectAction.engine=none;
                    break;
                case SDLK_s:
                    objectAction.engine=none;
                    break;
                case SDLK_d:
                    objectAction.maneuver=none;
                    break;
                case SDLK_a:
                    objectAction.maneuver=none;
                    break;
                case SDLK_SPACE:
                    objectAction.fireWeapon=none;
                default:
                    break;
            }
        }
    }
    shipMove(movement, ship);
    return flag;
}

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

void up_weaponCoolDown_start_setup()
{
    FILE *fp;
    fp = fopen("CoolDown.weapon", "r")
    if(fp==NULL)
    {
        UP_ERROR_MSG("Failed to open the cooldown file.");
        exit();
    }


}


