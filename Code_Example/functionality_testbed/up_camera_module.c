#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_objectReader.h"
#include <math.h>
#include <stdio.h>
// skapad av waleed hassan 2 maj 2015.

static float zoom = -2;

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

    // when the player do not have a ship yet
    if (ship == NULL) {
        return;
    }
    cam->center.x=ship->pos.x;
    cam->center.y=ship->pos.y;
    cam->center.z=ship->pos.z;
    cam->eye.x=ship->pos.x + ship->dir.x * zoom*10;
    cam->eye.y=ship->pos.y + ship->dir.y * zoom*10;
    cam->eye.z=ship->pos.z -10;


}


/*Function returns a struct with the object to be rendered for the client selectet by the distance between
 the camera center and every object in the solarsystem*/
 //This function was created when we used topview. We later decided to swap camera position and didnt have
 //the time to optimize objects rendered for a shoulder perspective. Objects behind the camera will also render.
 //Sebastian 2015-05-06
struct up_objectInfo *up_ObjectsInView(struct up_objectInfo *in_cam, int *count,struct up_camera *cam)
{
    int i,j=0;
    float distance=0, x=0, y=0, view=0, height=0;
    int totalObject = 0;

    struct up_objectInfo *allObj = up_unit_getAllObj(up_environment_type,&totalObject);

    height = up_returnCamHeight(cam);

    //value 0.7 comes from tan(70/2) where 70 is the set field of view of the camera from the top view
    //We later added the magic number 40 after we changed to a behind the shoulder camera position
    //that comes from playertesting
    view = 0.7 * height * 40;

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

        // shipIdx 0 is reserved for ships thats not active
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
