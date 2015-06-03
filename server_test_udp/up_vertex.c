#include "up_vertex.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "up_error.h"

float up_distance(struct up_vec3 a,struct up_vec3 b)
{
    float dx = a.x -b.x;
    float dy = a.y -b.y;
    float dz = a.z -b.z;
    
    return sqrtf(dx*dx + dy*dy + dz*dz);
}