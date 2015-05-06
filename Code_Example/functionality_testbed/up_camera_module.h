#ifndef UP_CAMERA_MODULE_H
#define UP_CAMERA_MODULE_H
#include "up_vertex.h"
#include "up_modelRepresentation.h"

    struct up_camera {
        struct up_vec3 eye;
        struct up_vec3 center;
        struct up_vec3 up;
    };

    struct up_perspective {
        float fov ;
        float aspectRatio;
        float zNear;
        float zFar;
    };

void up_cam_zoom(float change);

void up_update_camera(struct up_camera *cam,struct up_objectInfo *ship);
struct up_objectInfo *up_ObjectsInView(struct up_objectInfo *in_cam, int *count,struct up_camera *cam);

#endif
