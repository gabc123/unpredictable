//
//  up_modelRepresentation.h
//  Unpredictable
//
//  Created by Zetterman on 2015-04-16.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//

#ifndef UP_MODELREPRESENTATION_H
#define UP_MODELREPRESENTATION_H

#include "up_vertex.h"

struct shipMovement
{
    int up;
    int down;
    int left;
    int right;
};

struct up_ship
{
    struct up_vec3 pos;
    struct up_vec3 dir;
    float angle;
    float speed;
};


struct up_modelRepresentation
{
    struct up_vec3 pos;
    struct up_vec3 rot;
    struct up_vec3 scale;

};

#endif

