#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_objectReader.h"

// skapad av waleed hassan 2 maj 2015.

//
static float zoom = 0;

void up_cam_zoom(float change)
{
    zoom += change;
}


void up_update_camera(struct up_camera *cam,struct up_objectInfo *ship){
    // TODO: move camera
    cam->center.x=ship->pos.x;
    cam->center.y=ship->pos.y;
    cam->center.z=ship->pos.z;
    cam->eye.x=ship->pos.x;
    cam->eye.y=ship->pos.y;
    cam->eye.z=ship->pos.z - 20 + zoom;

    //cam->eye=ship->
}

struct up_objectInfo *up_ObjectsInView(int *count,struct up_camera *cam)
{
    return up_unit_getAllObj(count);
    
}