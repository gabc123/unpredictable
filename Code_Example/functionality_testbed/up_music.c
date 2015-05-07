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

#define NOTTRUE 0
#define TRUE 1
#define MAXSOUND 10


struct sound{
    Mix_Chunk *data[MAXSOUND];
    int nrOfTracks;
};

void up_music(int track){

    struct sound sound;
    
    int success;
   
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        success = NOTTRUE;
    }
    
    //Load music
    //Load background music
    sound.data[1] = Mix_LoadWAV( "Vangelis.wav" );
    
    if( sound.data[1] == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = NOTTRUE;
    }
    else sound.nrOfTracks++;
    
    
    //play music indefinitely
    Mix_PlayChannel( -1, sound.data[track], -1 );   //(channel, sound, loop)   -1 loop is infinite
    
    
}
