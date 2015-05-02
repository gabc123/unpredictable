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

struct internel_object_tracker{
    struct up_objectInfo *objects;
    int count;
    int size;
};


static struct internel_object_tracker internal_tracker;



int up_unit_start_setup(unsigned int max_unit_count)
{
    
    internal_tracker.count=0;
    struct up_objectInfo *objects = malloc(sizeof(struct up_objectInfo)*max_unit_count);
    
    if(objects == NULL){
        UP_ERROR_MSG("malloc failed");
        return 0;
    }
    
    struct up_objectInfo zero = {0};
    int i = 0;
    for (i = 0; i < max_unit_count; i++) {
        objects[i] = zero;
    }
    internal_tracker.objects = objects;
    internal_tracker.size=max_unit_count;
    
    
    return 1;
}

void up_unit_add(struct up_objectInfo object)
{
    int count=internal_tracker.count;
    
    if(count>=internal_tracker.size){
        UP_ERROR_MSG("unit buffer is full\n");
        return;
    }
    
    internal_tracker.objects[count]=object;
    
    internal_tracker.count++;
}

void up_unit_getAllObj(struct up_objectInfo *objectArray,int *count)
{
    objectArray = internal_tracker.objects;
    *count=internal_tracker.count;
    
}

void up_unit_shutdown_deinit()
{
    
    internal_tracker.size=0;
    internal_tracker.count=0;
    
    free(internal_tracker.objects);
    
    internal_tracker.objects=NULL;
    
}