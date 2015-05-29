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
    struct up_objectInfo *objectArray;
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
    internal_obj->objectArray = objArray;
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
    if (index >= objects->size) {
        UP_ERROR_MSG_INT("Tried to remove unit not in buffer\n",(int)objects->type);
        printf("\nUnit idx %d",index);
        return 0;
    }
    if(index>=objects->count){
        // unit not in use so return but just incase set to inacticve
        // objectId.idx == 0 means its not active
        objects->objectArray[index].objectId.idx = 0;
        return 0;
    }
    
    if (objects->objectArray[index].objectId.idx == 0) {
        printf("tried to remove unit that is already removed\n");
        return 1;
    }
    
    // objectId.idx == 0 means its not active
    objects->objectArray[index].objectId.idx = 0;
    
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


static int unit_add(struct internal_object * objectsArray,struct up_objectInfo object)
{
    int count=0;
    // first check if we have freed some object before that can be reused
    if (objectsArray->stack_top > 0) {
        objectsArray->stack_top--;
        count = objectsArray->availability_stack[objectsArray->stack_top];
        object.objectId.idx = count;
        
        objectsArray->objectArray[count]=object;
        
        return count;
        
    }

    // check if we can need to expand the thing
    count = objectsArray->count;
    if(count>=objectsArray->size){
        UP_ERROR_MSG_INT("unit buffer is full\n",(int)objectsArray->type);
        printf("\nUnit count %d",count);
        return 0;
    }

    object.objectId.idx = count;
    objectsArray->objectArray[count]=object;

    objectsArray->count++;
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

    if(objects->size <= index || index < 0){
        UP_ERROR_MSG_INT("try to access object out of bound",index);
        return NULL;
    }
    if (objects->count <= index) {
        return NULL;    // we have seperated out this so the server can use the same function, without error msg
    }
    struct up_objectInfo *obj = &objects->objectArray[index];
    
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
    return &objects->objectArray[0];
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


static int server_setObjAtIndex(struct internal_object * objectsArray, struct up_objectInfo object,int index)
{
    if (index >= objectsArray->size) {
        UP_ERROR_MSG_INT("Tried to set object at indx outofbounds", index);
        return 0;
    }
    object.objectId.idx = index;
    
    // first check if the object already exist, and if so set the object
    if (objectsArray->objectArray[index].objectId.idx != 0) {
        objectsArray->objectArray[index] = object;
        return 1;
    }
    
    
    int i = 0;
    
    // if the object is outside the current limits of the buffer expand it and then add our object at index
    if (index >= objectsArray->count) {
        // make sure all objects from objectsArray->count to index is set to inactive
        for (i = objectsArray->count; i < index; i++) {
            objectsArray->objectArray[i].objectId.idx = 0;
        }
        objectsArray->objectArray[index] = object;
        objectsArray->count = index + 1;    // we have now expanded the internal buffer and added our object at the right spot
        return 1;
    }
    
    
    
    int topIdx = 0;
    // we now need to reactivate the object, first check if it already have been removed once
    // then we use it, (we do not want to reactivate a unit directly and not remove it from here)
    int stack_top = objectsArray->stack_top;
    for (i = 0; i < stack_top; i++) {
        // have we found the index
        if (objectsArray->availability_stack[i] == index) {
            // this gets the top stack index and moves it to place i and then reduces the stack
            topIdx = objectsArray->availability_stack[stack_top - 1];
            objectsArray->availability_stack[i] = topIdx;
            objectsArray->stack_top--;
            objectsArray->objectArray[index] = object;
            return 1;
        }
    }

    return 0;
}


int up_server_unit_setObjAtindex(enum up_object_type type,struct up_objectInfo object,int index)
{
  
    switch (type) {
        case up_ship_type:
            server_setObjAtIndex(&internal_tracker.ships, object, index);
            break;
        case up_projectile_type:
            server_setObjAtIndex(&internal_tracker.projectile, object, index);

            break;
        case up_environment_type:
            server_setObjAtIndex(&internal_tracker.environment, object, index);

            break;
        case up_others_type:
            server_setObjAtIndex(&internal_tracker.others, object, index);

            break;
        default:
            UP_ERROR_MSG("Wrong type");
            break;
    }
    return 0;
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


    free(internal_tracker.ships.objectArray);
    free(internal_tracker.projectile.objectArray);
    free(internal_tracker.environment.objectArray);
    free(internal_tracker.others.objectArray);

    internal_tracker.ships.objectArray = NULL;
    internal_tracker.projectile.objectArray = NULL;
    internal_tracker.environment.objectArray = NULL;
    internal_tracker.others.objectArray = NULL;
    
    free(internal_tracker.ships.availability_stack);
    free(internal_tracker.projectile.availability_stack);
    free(internal_tracker.environment.availability_stack);
    free(internal_tracker.others.availability_stack);
    
    internal_tracker.ships.availability_stack = NULL;
    internal_tracker.projectile.availability_stack = NULL;
    internal_tracker.environment.availability_stack = NULL;
    internal_tracker.others.availability_stack = NULL;

}
