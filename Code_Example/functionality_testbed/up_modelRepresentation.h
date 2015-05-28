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
#include "up_type.h"

struct Hitbox{
    struct up_vec3 length;
};

struct shipMovement
{
    int up;
    int down;
    int left;
    int right;
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
    struct up_objectID objectId;
    struct up_vec3 scale;
    struct up_vec3 pos;
    struct up_vec3 dir;
    float angle;
    float bankAngle;
    float turnSpeed;
    float speed;
    float acceleration;
    struct Hitbox collisionbox;
    float maxLength;
    int owner;
    int projectile; //what kind of projectile
};




int up_unit_start_setup(unsigned int max_ship_count,unsigned int max_projectile_count,unsigned int max_environment_count,unsigned int max_others_count);
void up_unit_shutdown_deinit();

int up_unit_add(enum up_object_type type,struct up_objectInfo object);
int up_unit_remove(enum up_object_type type,int index);

int up_unit_isActive(struct up_objectInfo *object);

struct up_objectInfo *up_unit_objAtIndex(enum up_object_type type,int index);

struct up_objectInfo *up_unit_getAllObj(enum up_object_type type,int *count);


#endif

