//
//  up_network_module.h
//  testprojectshader
//
//  Created by o_0 on 2015-04-27.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_NETWORK_MODULE_H
#define UP_NETWORK_MODULE_H
#include "up_sdl_redirect.h"
#include "up_modelRepresentation.h"

void up_network_start_setup();
void up_network_shutdown_deinit();


void up_newtwork_getNewMovement(struct up_objectInfo * ship);




#endif /* defined(__testprojectshader__up_network_module__) */
