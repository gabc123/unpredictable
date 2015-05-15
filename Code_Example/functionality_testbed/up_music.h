//
//  up_music.h
//  Unpredictable
//
//  Created by Zetterman on 2015-05-07.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//

#ifndef __Unpredictable__up_music__
#define __Unpredictable__up_music__

#include <stdio.h>
#include "up_sdl_redirect.h"

#define MAXSOUND 10


struct soundLib{
    Mix_Chunk *data[MAXSOUND];
    int nrOfTracks;
    int toogleSoundEffects;
    int toogleThemeMusic;
};

struct soundLib *up_setupSound();
int up_music(int track, int loops, struct soundLib *sound);
void up_freeSound(struct soundLib *sound);


#endif /* defined(__Unpredictable__up_music__) */
