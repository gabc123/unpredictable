//
//  up_menu.c
//  Unpredictable
//
//  Created by Zetterman on 2015-04-28.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//

#include "up_menu.h"
#include <stdio.h>
#include "up_initGraphics.h"
#include "up_utilities.h"
#include "up_texture_module.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_shader_module.h"
#include "up_network_module.h"
#include "testmodels.h"

#include "up_sdl_redirect.h"  //mouse event handlingr

int up_menuEventHandler();


int up_menu(struct shader_module *shaderprog){
    
    int status=1;
    up_matrix4_t identity = up_matrix4identity();
    struct up_texture_data *textureMenuBackground = up_load_texture("1971687.png");
    if (textureMenuBackground==NULL) {
            textureMenuBackground = up_load_texture("lala.png");
    }
    
    //struct up_texture_data *textureBottonOne = up_load_texture("1971687.png");
    
    
    up_matrix4_t transform;

    struct up_mesh *background = up_meshMenuBackground();
    //struct up_mesh *bottonOne = up_meshbottonOne();
    
    while(status)
    {
        up_updateFrameTickRate();
        status = up_menuEventHandler();
        if (status==2) {
            break;
        }
        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //
        up_texture_bind(textureMenuBackground, 1);
        
        
        
        //do menu stuff
        
        
        
        //up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);
        
        up_getModelViewPerspective(&transform, &identity, &identity, &identity);
        
        //dispMat(&transform);
        UP_shader_update(shaderprog,&transform);
        
        up_draw_mesh(background);
       // up_draw_mesh(bottonOne);
        
        UP_openGLupdate();
        
        //transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
    }

    
    
    return status;
}


int up_menuEventHandler()
{
    int flag = 1;
    SDL_Event event;
    
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flag = 0;
        }
        if(event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_a:
                    flag=2;
                    break;
                    
                    /*  case SDLK_w:
                    movement->up = 1;
                    break;
                case SDLK_s:
                    movement->down = 1;
                    break;
                case SDLK_a:
                    movement->left=1;
                    break;
                case SDLK_d:
                    movement->right=1;
                    break;
                case SDLK_SPACE:
                    break;*/
                    
                default:
                    break;
            }
        }
        
        if(event.type == SDL_KEYUP)
        {
            switch (event.key.keysym.sym) {
                /*case SDLK_w:
                    movement->up=0;
                    break;
                case SDLK_s:
                    movement->down=0;
                    break;
                case SDLK_d:
                    movement->right=0;
                    break;
                case SDLK_a:
                    movement->left=0;
                    break;*/
                default:
                    break;
            }
        }
    }
    return flag;
}

