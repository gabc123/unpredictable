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

#define FALSE 0
#define TRUE 1


void closeMusic(Mix_Chunk *gMusic, Mix_Chunk *gExplosion);

void up_music(){
    
    
    
    
    int success;
    //int test;
    
    //The music that will be played
    Mix_Chunk *gMusic = NULL;
    
    //The sound effects that will be used
    Mix_Chunk *gExplosion = NULL;


    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        success = FALSE;
    }
    
    //Load music
    gMusic = Mix_LoadWAV( "Vangelis.wav" );
    
    if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = FALSE;
    }
    
    //Load sound effects
    gExplosion = Mix_LoadWAV( "" );
    if( gExplosion == NULL )
    {
        printf( "Failed to load explosion sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = FALSE;
    }
    
    //play music indefinitely
    Mix_PlayChannel( -1, gMusic, -1 );   //(channel, sound, loop)   -1 loop is infinite
    
    
}

void closeMusic(Mix_Chunk *gMusic, Mix_Chunk *gExplosion)
{
    
    //Free the sound effects
    Mix_FreeChunk( gExplosion );
    gExplosion = NULL;
    
    
    //Free the music
    Mix_FreeChunk( gMusic );
    gMusic = NULL;
    
    //Quit SDL subsystems
    Mix_Quit();
}
