//
//  up_modelRepresentation.c
//  Unpredictable
//
//  Created by Zetterman on 2015-04-16.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//
#include <stdlib.h>
#include "up_modelRepresentation.h"
#include "up_error.h"

struct internal_object
{
    struct up_objectInfo *objects;
    int count;
    int size;
};

struct internel_object_tracker
{
    struct internal_object ships;
    struct internal_object projectile;
    struct internal_object environment;
    struct internal_object others;
};



static struct internel_object_tracker internal_tracker;

int up_unit_start_setup(unsigned int max_ship_count,unsigned int max_projectile_count,unsigned int max_environment_count,unsigned int max_others_count)
{
    struct up_objectInfo zero = {0};
    int i = 0;

    //internal_tracker.count=0;
    struct up_objectInfo *ships = malloc(sizeof(struct up_objectInfo)*max_ship_count);

    if(ships == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }


    for (i = 0; i < max_ship_count; i++) {
        ships[i] = zero;
    }
    internal_tracker.ships.count = 0;
    internal_tracker.ships.objects = ships;
    internal_tracker.ships.size=max_ship_count;


    //////////////////

    //internal_tracker.count=0;
    struct up_objectInfo *projectile = malloc(sizeof(struct up_objectInfo)*max_projectile_count);

    if(projectile == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }


    for (i = 0; i < max_projectile_count; i++) {
        projectile[i] = zero;
    }
    internal_tracker.projectile.count = 0;
    internal_tracker.projectile.objects = projectile;
    internal_tracker.projectile.size=max_projectile_count;

    //////////////////

    //internal_tracker.count=0;
    struct up_objectInfo *environment = malloc(sizeof(struct up_objectInfo)*max_environment_count);

    if(environment == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }


    for (i = 0; i < max_environment_count; i++) {
        environment[i] = zero;
    }
    internal_tracker.environment.count = 0;
    internal_tracker.environment.objects = environment;
    internal_tracker.environment.size=max_environment_count;

    //////////////////

    //internal_tracker.count=0;
    struct up_objectInfo *others = malloc(sizeof(struct up_objectInfo)*max_others_count);

    if(others == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }


    for (i = 0; i < max_others_count; i++) {
        others[i] = zero;
    }
    internal_tracker.others.count = 0;
    internal_tracker.others.objects = others;
    internal_tracker.others.size=max_others_count;

    return 1;
}



static int unit_add(struct internal_object * objects,struct up_objectInfo object)
{
    int count=objects->count;

    if(count>=objects->size){
        UP_ERROR_MSG("unit buffer is full\n");
        return 0;
    }

    object.objectId.idx = count;
    objects->objects[count]=object;

    objects->count++;
    return count;
}

int up_unit_add(enum up_object_type type,struct up_objectInfo object)
{
    int index = 0;
    object.objectId.type = type;
    switch (type) {
        case up_ship_type:
            index = unit_add(&internal_tracker.ships, object);
            break;
        case up_projectile_type:
            index = unit_add(&internal_tracker.projectile, object);
            break;
        case up_environment_type:
            index = unit_add(&internal_tracker.environment, object);
            break;
        case up_others_type:
            index = unit_add(&internal_tracker.others, object);
            break;
        default:
            UP_ERROR_MSG("Wrong type");
            break;
    }
    return index;
}


static struct up_objectInfo *unit_objAtIndex(struct internal_object * objects, int index){

    if(objects->count <= index){
        UP_ERROR_MSG("try to access object out of bound");
        return NULL;
    }

    return &objects->objects[index];
}

struct up_objectInfo *up_unit_objAtIndex(enum up_object_type type,int index)
{
    struct up_objectInfo *object = NULL;
    switch (type) {
        case up_ship_type:
            object = unit_objAtIndex(&internal_tracker.ships, index);
            break;
        case up_projectile_type:
            object = unit_objAtIndex(&internal_tracker.projectile, index);
            break;
        case up_environment_type:
            object = unit_objAtIndex(&internal_tracker.environment, index);
            break;
        case up_others_type:
            object = unit_objAtIndex(&internal_tracker.others, index);
            break;
        default:
            UP_ERROR_MSG("Wrong type");
            break;
    }

    return object;
}


static struct up_objectInfo *unit_getAllObj(struct internal_object * objects,int *count)
{
    *count=objects->count;
    return &objects->objects[0];
}

struct up_objectInfo *up_unit_getAllObj(enum up_object_type type,int *count)
{
    struct up_objectInfo *object = NULL;
    switch (type) {
        case up_ship_type:
            object = unit_getAllObj(&internal_tracker.ships, count);
            break;
        case up_projectile_type:
            object = unit_getAllObj(&internal_tracker.projectile, count);
            break;
        case up_environment_type:
            object = unit_getAllObj(&internal_tracker.environment, count);
            break;
        case up_others_type:
            object = unit_getAllObj(&internal_tracker.others, count);
            break;
        default:
            UP_ERROR_MSG("Wrong type");
            break;
    }

    return object;
}



void up_unit_shutdown_deinit()
{

    internal_tracker.ships.size=0;
    internal_tracker.ships.count=0;

    internal_tracker.projectile.size=0;
    internal_tracker.projectile.count=0;

    internal_tracker.environment.size=0;
    internal_tracker.environment.count=0;

    free(internal_tracker.ships.objects);
    free(internal_tracker.projectile.objects);
    free(internal_tracker.environment.objects);
    free(internal_tracker.others.objects);

    internal_tracker.ships.objects = NULL;
    internal_tracker.projectile.objects = NULL;
    internal_tracker.environment.objects = NULL;
    internal_tracker.others.objects = NULL;

}
