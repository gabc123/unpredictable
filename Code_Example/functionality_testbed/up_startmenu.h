//
//  up_startmenu.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-16.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_STARTMENU_H
#define UP_STARTMENU_H

#include "up_assets.h"
#include "up_music.h"
#include "up_network_module.h"

int up_startmenu(struct up_map_data *mapData,
                 struct up_key_map *keymap,
                 struct up_network_datapipe *connection,
                 struct soundLib *sound,
                 struct up_font_assets *fonts,
                 struct up_shader_module *shader);

#endif
