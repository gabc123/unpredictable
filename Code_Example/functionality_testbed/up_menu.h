//
//  up_menu.h
//  Unpredictable
//
//  Created by Zetterman on 2015-04-28.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//

#ifndef __Unpredictable__up_menu__
#define __Unpredictable__up_menu__

#include <stdio.h>
#include "up_music.h"
#include "up_shader_module.h"
#include "up_ship.h"
#include "up_assets.h"
#include "sha256.h"

int up_menu(struct shader_module *shaderprog, struct soundLib *sound,struct up_key_map *keymap,struct up_font_assets *fonts);

#endif /* defined(__Unpredictable__up_menu__) */
