#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_objectReader.h"

// skapad av waleed hassan 2 maj 2015.



void up_update_camera(struct up_camera *cam,struct up_ship *ship){
    // TODO: move camera
    cam->center.x=ship->pos.x;
    cam->center.y=ship->pos.y;
    //cam->center.z=ship->pos.z - 10;
    cam->eye.x=ship->pos.x;
    cam->eye.y=ship->pos.y;
    cam->eye.z=ship->pos.z - 100;

    //cam->eye=ship->
}

void up_ObjectsInView(struct up_objectInfo *objectArray,int *count,struct up_camera *cam)
{
    up_unit_getAllObj(objectArray, count);
    
}