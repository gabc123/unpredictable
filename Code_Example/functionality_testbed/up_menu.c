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


enum menu_states
{
    mainMenu,
    loginMenu,
    
};

struct navigationState{
    enum menu_states state;
};


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
    struct up_texture_data *textureLoginOverlay = up_load_texture("placeholder-login2.png");
    if (textureLoginOverlay==NULL) {
        textureLoginOverlay = up_load_texture("lala.png");
    }
    
    
    
    up_matrix4_t transformLoginRegisterBottons;
    up_matrix4_t transformBackground;
    up_matrix4_t transformLoginOverlay;
    
    up_matrix4_t translationLoginRegisterButtons;
    up_matrix4_t translationLoginOverlay;

    struct up_mesh *background = up_meshMenuBackground();
    struct up_mesh *bottonLogin1 =up_meshBotton(0,0.95,0,0); //(float imageX, float imageY, float screenPosX, float screenPosY)
    struct up_mesh *bottonLogin2 =up_meshBotton(0,0.1,0,-0.25);
    struct up_mesh *overlay = up_meshLoginOverlay();
    
    //LOGIN AND REGISTER BUTTONS
    struct up_modelRepresentation scale1 ={{0,0,0},     //changes the scale of the bottons to x0.5
                                          {0,0,0},
                                          {0.5,0.5,0.5}};
    
    
    up_matrixModel(&translationLoginRegisterButtons, &scale1.pos, &scale1.rot, &scale1.scale);
    
    up_getModelViewPerspective(&transformLoginRegisterBottons, &translationLoginRegisterButtons, &identity, &identity);
    
    //BACKGROUND
    up_getModelViewPerspective(&transformBackground, &identity, &identity, &identity);
    
    //LOGIN OVERLAY
    struct up_modelRepresentation scale2 ={{0,0,0},     //no scale (yet)
                                           {0,0,0},
                                           {0.95,0.95,0.95}};
    
    
    up_matrixModel(&translationLoginOverlay, &scale2.pos, &scale2.rot, &scale2.scale);
    
    up_getModelViewPerspective(&transformLoginOverlay, &translationLoginOverlay, &identity, &identity);
    
    struct navigationState navigation;
    
    navigation.state = mainMenu;
    
    
    while(status)
    {
        up_updateFrameTickRate();
        status = up_menuEventHandler(&navigation);
        if (status==2) {
            break;
        }
        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //


        //do menu stuff



        //up_matrixView(&viewMatrix, &cam.eye, &cam.center, &cam.up);

        //up_getModelViewPerspective(&transform, &translation, &identity, &identity);

        //dispMat(&transform);
        UP_shader_update(shaderprog,&transformBackground);    //background
        up_texture_bind(textureMenuBackground, 1);
        //up_shader_update_sunligth(shaderprog,&identity);
        up_draw_mesh(background);
        
        
        switch (navigation.state) {
            case mainMenu:
                UP_shader_update(shaderprog,&transformLoginRegisterBottons);     //botton1
                up_texture_bind(textureBottonLogin, 2);
                up_draw_mesh(bottonLogin1);
                
                UP_shader_update(shaderprog,&transformLoginRegisterBottons);     //botton2
                up_texture_bind(textureBottonLogin, 2);
                up_draw_mesh(bottonLogin2);
                
                
                break;
                
            case loginMenu:
                UP_shader_update(shaderprog, &transformLoginOverlay);
                up_texture_bind(textureLoginOverlay, 3);
                up_draw_mesh(overlay);
                
            default:
                break;
        }
        
        
    
        
        UP_openGLupdate();

        //transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
    }



    return status;
}


int up_menuEventHandler(struct navigationState *navigation)
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
                    
                    //aspect ratio
                    xf=(float)x/width*2-1;
                    yf=-(float)y/height*2+1;


                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of login screen
                        if(yf > 0.047727 && yf < 0.131818){
                            
                            navigation->state = loginMenu;
                        }
                    }

                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of registration screen
                        if(yf > -0.068182 && yf < 0.015909){

                            printf("Register botton clicked!\n");
                            flag=2;
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

