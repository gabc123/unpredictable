//
//  up_type.h
//  testprojectshader
//
//  Created by o_0 on 2015-05-08.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_TYPE_H
#define UP_TYPE_H

enum up_object_type
{
    up_ship_type = 1,
    up_projectile_type,
    up_environment_type,
    up_others_type
};

struct up_objectID
{
    int idx;
    enum up_object_type type;
};

#endif
