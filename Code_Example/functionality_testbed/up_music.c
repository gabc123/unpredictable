//
//  up_music.c
//  Unpredictable
//
//  Created by Zetterman on 2015-05-07.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//

#include "up_music.h"

#include "up_sdl_redirect.h"

#include <stdio.h>

#include <assert.h>


#define MAXSOUND 10


int createSound(struct soundLib *sound, Mix_Chunk *data ){
    
    int success = 1;
    
    sound->data[sound->nrOfTracks] = data;
    
    sound->nrOfTracks += 1;
    
    if(sound->nrOfTracks == 0){
        
        success= -1;
        printf("ERROR: No tracks \n");
    }
    
    return success;
}



//NEED TO FIX POINTER TO STRUCT

struct soundLib *up_setupSound(){
    
    //INIT SOUND
    
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }
    
    struct soundLib *sound;
    
    sound = malloc(sizeof(struct soundLib));
    
    assert(sound != NULL);
    
    sound->nrOfTracks = 0;
    sound->toogleSoundEffects = 1;
    sound->toogleThemeMusic = 1;

    
    //CREATING SOUND
    
    createSound(sound, Mix_LoadWAV( "Vangelis.wav" ));
    createSound(sound, Mix_LoadWAV("Laser_Sound_Effect.wav"));
    
    //sound = createSound(Mix_LoadWAV("Laser_Sound_Effect.wav"), 2);
    
    return sound;
}



void up_freeSound(struct soundLib *sound){
    
    assert(sound != NULL);

    free(sound);
}


int up_music(int track, int loops, struct soundLib *sound){
    
    
    int channel = track;
    
    int success=1;
    
    if (sound->toogleSoundEffects == 0  &&  track == 1) {
        return success;
    }
        
    else{
         success = Mix_PlayChannel( channel, sound->data[track], loops );   //(channel, sound, loop)   -1 loop is infinite
    }

    
    
    return success;
    
}
