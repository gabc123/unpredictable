#ifndef UP_SDL_REDIRECT_H
#define UP_SDL_REDIRECT_H

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#endif // __APPLE__
#endif // UP_SDL_REDIRECT_H
