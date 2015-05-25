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
    int *availability_stack;
    int stack_top;
    enum up_object_type type;
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

static int up_internal_object_init(struct internal_object *internal_obj,enum up_object_type type,unsigned int max_count)
{
    struct up_objectInfo zero = {0};
    int i = 0;
    
    //internal_tracker.count=0;
    struct up_objectInfo *objArray = malloc(sizeof(struct up_objectInfo)*max_count);
    if(objArray == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    int *availability_stack = malloc(sizeof(int)*max_count);
    if (availability_stack == NULL) {
        UP_ERROR_MSG("malloc available stack failed");
        free(objArray);
        return 0;
    }
    
    
    for (i = 0; i < max_count; i++) {
        objArray[i] = zero;
        availability_stack[i] = 0;
    }

    
    internal_obj->count = 1;
    internal_obj->objects = objArray;
    internal_obj->size = max_count;
    
    internal_obj->availability_stack = availability_stack;
    internal_obj->stack_top = 0;
    
    internal_obj->type = type;
    return 1;
}

int up_unit_start_setup(unsigned int max_ship_count,unsigned int max_projectile_count,unsigned int max_environment_count,unsigned int max_others_count)
{
    int err = 0;
    
    err = up_internal_object_init(&internal_tracker.ships, up_ship_type, max_ship_count);
    if (err == 0) {
        UP_ERROR_MSG("failed unit startup");
        return 0;
    }
    
    err = up_internal_object_init(&internal_tracker.projectile, up_projectile_type, max_projectile_count);
    if (err == 0) {
        UP_ERROR_MSG("failed unit startup");
        return 0;
    }
    
    err = up_internal_object_init(&internal_tracker.environment, up_environment_type, max_environment_count);
    if (err == 0) {
        UP_ERROR_MSG("failed unit startup");
        return 0;
    }
    
    err = up_internal_object_init(&internal_tracker.others, up_others_type, max_others_count);
    if (err == 0) {
        UP_ERROR_MSG("failed unit startup");
        return 0;
    }
    return 1;
}

static int unit_remove(struct internal_object * objects,int index)
{
    if(index>=objects->count){
        UP_ERROR_MSG_INT("Tried to remove unit not in buffer\n",(int)objects->type);
        printf("\nUnit count %d",index);
        return 0;
    }
    
    if (objects->objects[index].objectId.idx == 0) {
        printf("tried to remove unit that is already removed\n");
        return 1;
    }
    
    // objectId.idx == 0 means its not active
    objects->objects[index].objectId.idx = 0;
    
    if (objects->stack_top >= objects->size) {
        UP_ERROR_MSG_INT("availability_stack is full, big bad bug",(int)objects->type);
        
    }
    
    // availability_stack keeps track of all fread objects, so they can be reused
    objects->availability_stack[objects->stack_top] = index;
    objects->stack_top++;
    
    return 1;
}

int up_unit_remove(enum up_object_type type,int index)
{
    int err = 0;
    switch (type) {
        case up_ship_type:
            err = unit_remove(&internal_tracker.ships, index);
            break;
        case up_projectile_type:
            err = unit_remove(&internal_tracker.projectile, index);
            break;
        case up_environment_type:
            err = unit_remove(&internal_tracker.environment, index);
            break;
        case up_others_type:
            err = unit_remove(&internal_tracker.others, index);
            break;
        default:
            err = 0;
            UP_ERROR_MSG("Wrong type");
            break;
    }
    return err;
}


static int unit_add(struct internal_object * objects,struct up_objectInfo object)
{
    int count=0;
    // first check if we have freed some object before that can be reused
    if (objects->stack_top > 0) {
        objects->stack_top--;
        count = objects->availability_stack[objects->stack_top];
        object.objectId.idx = count;
        
        objects->objects[count]=object;
        
        return count;
        
    }

    // check if we can need to expand the thing
    count = objects->count;
    if(count>=objects->size){
        UP_ERROR_MSG_INT("unit buffer is full\n",(int)objects->type);
        printf("\nUnit count %d",count);
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
    struct up_objectInfo *obj = &objects->objects[index];
    
    // if the idx is 0 then this object is not active and has been removed, so return NULL instead
    obj = (obj->objectId.idx != 0) ? obj : NULL;
    
    return obj;
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
            UP_ERROR_MSG_INT("Wrong type",(int)type);
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

// check if a unit is active, this gives a abstraction if we want to change how it works internaly
int up_unit_isActive(struct up_objectInfo *object)
{
    return (object->objectId.idx != 0);
}

void up_unit_shutdown_deinit()
{

    
    internal_tracker.ships.size = 0;
    internal_tracker.ships.count = 0;
    internal_tracker.ships.stack_top = 0;
    
    internal_tracker.projectile.size = 0;
    internal_tracker.projectile.count = 0;
    internal_tracker.projectile.stack_top = 0;
    
    internal_tracker.environment.size = 0;
    internal_tracker.environment.count = 0;
    internal_tracker.environment.stack_top = 0;


    free(internal_tracker.ships.objects);
    free(internal_tracker.projectile.objects);
    free(internal_tracker.environment.objects);
    free(internal_tracker.others.objects);

    internal_tracker.ships.objects = NULL;
    internal_tracker.projectile.objects = NULL;
    internal_tracker.environment.objects = NULL;
    internal_tracker.others.objects = NULL;
    
    free(internal_tracker.ships.availability_stack);
    free(internal_tracker.projectile.availability_stack);
    free(internal_tracker.environment.availability_stack);
    free(internal_tracker.others.availability_stack);
    
    internal_tracker.ships.availability_stack = NULL;
    internal_tracker.projectile.availability_stack = NULL;
    internal_tracker.environment.availability_stack = NULL;
    internal_tracker.others.availability_stack = NULL;

}
