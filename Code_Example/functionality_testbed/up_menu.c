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
#include "up_modelRepresentation.h"
#include "up_matrixTransforms.h"



#include "up_sdl_redirect.h"  //mouse event handlingr

int up_menuEventHandler();


int up_menu(struct shader_module *shaderprog){

    int status=1;
    up_matrix4_t identity = up_matrix4identity();
    struct up_texture_data *textureMenuBackground = up_load_texture("1971687.png");
    if (textureMenuBackground==NULL) {
            textureMenuBackground = up_load_texture("lala.png");
    }

    struct up_texture_data *textureBottonLogin = up_load_texture("menuBottons.png");
    if (textureBottonLogin==NULL) {
            textureBottonLogin = up_load_texture("lala.png");
    }

    up_matrix4_t transform1;
    up_matrix4_t transform2;

    struct up_mesh *background = up_meshMenuBackground();
    struct up_mesh *bottonLogin1 =up_meshBotton(0,0.95,0,0); //(float imageX, float imageY, float screenPosX, float screenPosY)
    struct up_mesh *bottonLogin2 =up_meshBotton(0,0.1,0,-0.25);

    struct up_modelRepresentation scale ={{0,0,0},     //changes the scale of the bottons to x0.5
                                          {0,0,0},
                                          {0.5,0.5,0.5}};

    up_matrix4_t translation;
    up_matrixModel(&translation, &scale.pos, &scale.rot, &scale.scale);
    up_getModelViewPerspective(&transform1, &translation, &identity, &identity);


    up_getModelViewPerspective(&transform2, &identity, &identity, &identity);


    while(status)
    {
        up_updateFrameTickRate();
        status = up_menuEventHandler();
        if (status==2) {
            break;
        }
        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //


        //do menu stuff



        //up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

        //up_getModelViewPerspective(&transform, &translation, &identity, &identity);

        //dispMat(&transform);
        UP_shader_update(shaderprog,&transform2);    //background
        up_texture_bind(textureMenuBackground, 1);
        up_draw_mesh(background);

        UP_shader_update(shaderprog,&transform1);     //botton1
        up_texture_bind(textureBottonLogin, 2);
        up_draw_mesh(bottonLogin1);

        UP_shader_update(shaderprog,&transform1);     //botton2
        up_texture_bind(textureBottonLogin, 2);
        up_draw_mesh(bottonLogin2);

        UP_openGLupdate();

        //transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
    }



    return status;
}


int up_menuEventHandler()
{
    int flag = 1;
    int x,y;
    float xf,yf;
    float width = 1280;
    float height = 880;
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



                    /*
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
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    SDL_GetMouseState(&x, &y);

                    xf=(float)x/width*2-1;
                    yf=-(float)y/height*2+1;


                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of login screen
                        if(yf > 0.047727 && yf < 0.131818){

                            flag=2;
                        }
                    }

                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of registration screen
                        if(yf > -0.068182 && yf < 0.015909){

                            printf("Register botton clicked!\n");
                        }
                    }

                    break;
                    /*
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
    }
    return flag;
}

