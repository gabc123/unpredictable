#ifndef UP_CAMERA_MODULE_H
#define UP_CAMERA_MODULE_H
#include "up_vertex.h"

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

#endif
