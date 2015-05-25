#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_objectReader.h"
#include <math.h>
#include <stdio.h>
// skapad av waleed hassan 2 maj 2015.

static float zoom = 0;

void up_cam_zoom(float change)
{
    zoom += change;
}

//sebastian 2015-05-07
float up_returnCamHeight(struct up_camera *cam)
{
    float height = (zoom < 0 ) ? -zoom : zoom;
    height +=cam->center.z - cam->eye.z;
    return height;
}


void up_update_camera(struct up_camera *cam,struct up_objectInfo *ship){
    // TODO: move camera
    cam->center.x=ship->pos.x;
    cam->center.y=ship->pos.y;
    cam->center.z=ship->pos.z;
    cam->eye.x=ship->pos.x;
    cam->eye.y=ship->pos.y;
    cam->eye.z=ship->pos.z - 80 + zoom;

    //cam->eye=ship->
}

/*
struct up_objectInfo
{
    int modelId;
    struct up_vec3 scale;
    struct up_vec3 pos;
    struct up_vec3 dir;
    float angle;
    float turnSpeed;
    float speed;
    float acceleration;
};
*/

/*Function returns a struct with the object to be rendered for the client selectet by the distance between
 the camera center and every object in the solarsystem*/
 //Sebastian 2015-05-06
struct up_objectInfo *up_ObjectsInView(struct up_objectInfo *in_cam, int *count,struct up_camera *cam)
{
    int i,j=0;
    float distance=0, x=0, y=0, view=0, height=0;
    int totalObject = 0;

    struct up_objectInfo *allObj = up_unit_getAllObj(up_environment_type,&totalObject);

    height = up_returnCamHeight(cam);

    //value 0.7 comes from tan(70/2) where 70 is the set field of view of the camera
    view = 0.7 * height;

    for(i=0;i<totalObject;i++){
        if (!up_unit_isActive(&allObj[i])) {
            continue;
        }
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        distance=sqrt((x*x)+(y*y));

        if(distance<view)
            in_cam[j++]=allObj[i];

    }

    allObj = up_unit_getAllObj(up_ship_type,&totalObject);
    for(i=0;i<totalObject;i++){
        if (!up_unit_isActive(&allObj[i])) {
            continue;
        }
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        distance=sqrt((x*x)+(y*y));

        // shipIdx 0 is reserved for ship thats not active
        if(distance<view && (allObj[i].objectId.idx != 0))
        {
            in_cam[j++]=allObj[i];
        }
    }

    allObj = up_unit_getAllObj(up_projectile_type,&totalObject);

    for(i=0;i<totalObject;i++){
        if (!up_unit_isActive(&allObj[i])) {
            continue;
        }
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        distance=sqrt((x*x)+(y*y));

        if(distance<view)
            in_cam[j++]=allObj[i];

    }

    allObj = up_unit_getAllObj(up_others_type,&totalObject);
    for(i=0;i<totalObject;i++){
        if (!up_unit_isActive(&allObj[i])) {
            continue;
        }
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;

        distance=sqrt((x*x)+(y*y));
        if(distance<view)
            in_cam[j++]=allObj[i];

    }

    *count = j;

    return in_cam;
    //return up_unit_getAllObj(count);

}
