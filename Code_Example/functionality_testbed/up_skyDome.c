////
////  up_skyDome.c
////  up_game
////
////  Created by Waleed Hassan on 2015-05-11.
////  Copyright (c) 2015 Waleed Hassan. All rights reserved.
////
//
//#include "up_skyDome.h"
//#include "up_texture_module.h"
//#include "up_sdl_redirect.h"
//#include "up_opengl_redirect.h"
//#include "up_error.h"
//
//
//
//
//
//
//int up_cubeMapTexture_load(){
//    
//    struct up_texture_data textureId;
//
//    
//    glGenTextures(1, &textureId.textureId);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId.textureId);
//    
//    SDL_Surface *texF = IMG_Load("front.png");
//    SDL_Surface *texT = IMG_Load("top.png");
//    SDL_Surface *texG = IMG_Load("bottom.png");
//    SDL_Surface *texR = IMG_Load("right.png");
//    SDL_Surface *texL = IMG_Load("left.png");
//    SDL_Surface *texB = IMG_Load("back.png");
//    
//    if (texF->format->BytesPerPixel != 4) {
//        
//        UP_ERROR_MSG("Error: picture is in wrong pixel format");
//        
//    }
//    
//    GLuint format_rgb = GL_RGBA;
//    #ifdef __linux
//    format_rgb = GL_RGBA;
//    #else
//    format_rgb = GL_BGRA;
//    #endif // __linux
//    
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, texF->w, texF->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texF->pixels);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, texT->w, texT->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texT->pixels);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, texG->w, texG->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texG->pixels);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, texR->w, texR->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texR->pixels);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, texL->w, texL->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texL->pixels);
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, texB->w, texB->h, 0, format_rgb, GL_UNSIGNED_INT_8_8_8_8_REV, texB->pixels);
//    
//
//    
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    
//    
//    return 0;
//}