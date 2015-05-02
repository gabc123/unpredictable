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

struct up_objectInfo
{
    int modelId;
    struct up_vec3 scale;
    struct up_vec3 pos;
    struct up_vec3 dir;
    float angle;
    float speed;
};

int up_unit_start_setup(unsigned int max_unit_count);
void up_unit_shutdown_deinit();
void up_unit_add(struct up_objectInfo object);
void up_unit_getAllObj(struct up_objectInfo *objectArray,int *count);


#endif

