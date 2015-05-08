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


void up_generate_sun();
void up_generate_asteroidBelt(int density,float maxAngle,float minAngle,float outerEdge,float innerEdge,float maxHeight,float minHeight);
void up_generate_randomize_satellite(int density);

#endif /* defined(__up_game__up_star_system__) */
