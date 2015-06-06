//
//  up_star_system.h
//  up_game
//
//  Created by Waleed Hassan on 2015-05-03.
//  Copyright (c) 2015 Waleed Hassan. All rights reserved.
//

#ifndef __up_game__up_star_system__
#define __up_game__up_star_system__

#include <stdio.h>
#include "up_render_engine.h"


// generate the map from the seed, given by the server

struct up_sun_shell
{
    struct up_vec3 sunColor;
    struct up_vec3 ligthDropoff;
    float dropOff_dir;              //1.0 == transparent egde, 0.0 == transparent center
    up_matrix4_t modelTransforms;
    struct up_modelOrientation orentation;
};

struct up_sun
{
    struct up_render_metaData render;
    struct up_sun_shell shells[2];
    
};

struct up_sun up_sun_create(struct shader_module *shader);
void up_update_sun(struct up_sun *sun);

void up_generate_map(int seed);



#endif /* defined(__up_game__up_star_system__) */
