#include "up_camera_module.h"
#include "up_matrixTransforms.h"
#include "up_objectReader.h"
#include <math.h>
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
    float distance=0,x=0,y=0;
    int totalObject = 0;
    
    struct up_objectInfo *allObj = up_unit_getAllObj(up_environment_type,&totalObject);

    for(i=0;i<totalObject;i++){

        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;

        distance=sqrt((x*x)+(y*y));

        if(distance<200)
            in_cam[j++]=allObj[i];

    }

    allObj = up_unit_getAllObj(up_ship_type,&totalObject);
    *count = j;
    for(i=0;i<totalObject;i++){
        
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        
        distance=sqrt((x*x)+(y*y));
        
        if(distance<200)
            in_cam[j++]=allObj[i];
        
    }

    allObj = up_unit_getAllObj(up_projectile_type,&totalObject);
    *count = j;
    for(i=0;i<totalObject;i++){
        
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        
        distance=sqrt((x*x)+(y*y));
        
        if(distance<200)
            in_cam[j++]=allObj[i];
        
    }
    
    allObj = up_unit_getAllObj(up_others_type,&totalObject);
    *count = j;
    for(i=0;i<totalObject;i++){
        
        x = cam->center.x - allObj[i].pos.x;
        y = cam->center.y - allObj[i].pos.y;
        
        distance=sqrt((x*x)+(y*y));
        
        if(distance<200)
            in_cam[j++]=allObj[i];
        
    }

    
    return in_cam;
    //return up_unit_getAllObj(count);

}
