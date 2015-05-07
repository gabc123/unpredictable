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
#include "up_assets.h"
#include "up_utilities.h"
#include "up_texture_module.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_shader_module.h"
#include "up_network_module.h"
#include "testmodels.h"
#include "up_modelRepresentation.h"
#include "up_matrixTransforms.h"
#include "up_music.h"

#define UP_LIMIT 30


#include "up_sdl_redirect.h"  //mouse event handlingr


enum menu_states
{
    mainMenu,
    loginMenu,
    usernameBar,
    quitWindow
    
    
};

enum loginBar_state
{
    writeOn,
    writeOff
};

struct navigationState{
    enum menu_states state;
    enum loginBar_state status;
};

struct userData{
    char username[UP_LIMIT];
    char password[UP_LIMIT];
    int keypress;
};


int up_menuEventHandler(struct navigationState *navigation, struct navigationState *loginBar, struct userData *user_data);


int up_menu(struct shader_module *shaderprog){

    int status=1;
    
    up_music();
    
    //IMAGE LOADING
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
    
    struct up_texture_data *textureQuiteWindow = up_load_texture("quitWindow.png");
    if (textureQuiteWindow==NULL) {
        textureQuiteWindow = up_load_texture("lala.png");
    }
    
    
    //TRANSFORMS
    up_matrix4_t transformLoginRegisterBottons;
    up_matrix4_t transformBackground;
    up_matrix4_t transformLoginOverlay;
    up_matrix4_t transformQuiteWindow;

    
    //TRANSLATION TRANSFORMS
    up_matrix4_t translationLoginRegisterButtons;
    up_matrix4_t translationLoginOverlay;
    up_matrix4_t translationQuiteWindow;


    //MESH LOADING
    struct up_mesh *background = up_meshMenuBackground();
    struct up_mesh *bottonLogin1 =up_meshBotton(0,0.95,0,0); //(float imageX, float imageY, float screenPosX, float screenPosY)
    struct up_mesh *bottonLogin2 =up_meshBotton(0,0.1,0,-0.25);
    struct up_mesh *overlay = up_meshLoginOverlay();
    struct up_mesh *quiteWindow = up_meshQuitwindow();
    
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

    
    
    //QUIT WINDOW
    
    struct up_modelRepresentation scale3 ={{0,0,0},     //scaling
                                           {0,0,0},
                                           {0.7,0.7,0.7}};
    
    
    up_matrixModel(&translationQuiteWindow, &scale3.pos, &scale3.rot, &scale3.scale);
    
    up_getModelViewPerspective(&transformQuiteWindow, &translationQuiteWindow, &identity, &identity);
    
    
    
    //FONT
    struct up_font_assets *fonts = up_font_start_setup();
    struct up_vec3 textpos = {-0.17, 0.045, 0};
    struct up_vec3 textscale = {0.025,0.025,0.025};
    
    
    
    //NAVIGATION
    struct navigationState navigation;
    struct navigationState loginBar;
    
    navigation.state = mainMenu;
    loginBar.status = writeOff;
    
    
    //USER DATA
    struct userData user_data;
    user_data.keypress=0;
    
    // MENU LOOP
    while(status)
    {
        up_updateFrameTickRate();
        
        
        
        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //
        
        status = up_menuEventHandler(&navigation, &loginBar, &user_data);
        //do menu stuff

        if (status==2) {
            break;
        }

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
            
                
                if (navigation.status == writeOn) {
                    up_displayText(user_data.username, user_data.keypress, &textpos, &textscale, fonts, shaderprog);
                    
                }
                
                
                break;
                
            case quitWindow:
                
                UP_shader_update(shaderprog, &transformQuiteWindow);
                up_texture_bind(textureQuiteWindow, 2);
                up_draw_mesh(quiteWindow);
            
                break;
              
            default:
                break;
        }
        
        
    
        
        UP_openGLupdate();

        //transform = up_matrixModel(&model.pos, &model.rot, &model.scale);
    }



    return status;
}


int up_menuEventHandler(struct navigationState *navigation, struct navigationState *loginBar, struct userData *user_data)
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

        if(event.type == SDL_KEYUP)
        {
            if ((navigation->status == writeOn) && (user_data->keypress <= 30)) {
                
                //int data = event.key.keysym.sym;
                //printf("%d \n", data);
                
                if ((event.key.keysym.sym > 32) && ( event.key.keysym.sym < 127)) { // from a to z
                    user_data->username[user_data->keypress] = event.key.keysym.sym;
                    user_data->keypress++;
                }
                if (event.key.keysym.sym == 32) {  //FOR TESTING, SPACE ERASES ALL
                    user_data->username[UP_LIMIT - 1] = '\0';
                    printf("%s\n",user_data->username);
                    user_data->keypress = 0;
                }
                if (event.key.keysym.sym == 8){  //BACKSPACE
                    if (user_data->keypress != 0) {
                        user_data->keypress = user_data->keypress - 1;
                        
                        user_data->username[user_data->keypress] = '\0';
                    }
                }
                
                
            
                
            }
            
            if (event.key.keysym.sym == 27) {
                
                if (navigation->state == mainMenu) {
                    navigation->state= quitWindow;
                }
            
                if (navigation->state == loginMenu) {
                    navigation->state= mainMenu;
                }
            }

            
        }
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    SDL_GetMouseState(&x, &y);
                    
                    //aspect ratio
                    xf=(float)x/width*2-1;
                    yf=-(float)y/height*2+1;

                    printf("X AND Y COORDINATES: %f %f \n", xf, yf); //test print
                    
                    
                    //LOGIN BOTTON
                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of login screen
                        if(yf > 0.047727 && yf < 0.131818){
                            
                            if(navigation->state == mainMenu){
                                 navigation->state = loginMenu;
                            }
                        }
                    }
                    
                    //REGISTER BOTTON
                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of registration screen
                        if(yf > -0.068182 && yf < 0.015909){
                            
                            if (navigation->state == mainMenu){
                                printf("Register botton clicked!\n");
                                flag=2;
                            }
                        }
                    }
                    
                    //WRITE IN LOGIN BAR
                    if(xf > -0.198438 && xf < 0.195312){
                        if(yf > 0.093182 && yf < 0.175000){
                            
                            if (navigation->state == loginMenu){
                                navigation->status=writeOn;
                            }
                        }
                    }
                    
                    //QUIT WINDOW
                    
                    if(xf > -0.151563 && xf < -0.012500){          //TURN OFF
                        if (yf > -0.029545 && yf < 0.072727){
                            
                            if (navigation->state == quitWindow) {
                                flag=0;
                                printf("QUITING \n");
                            }
                        }
                    }
                    
                    if(xf > 0.012500 && xf < 0.151562){            //CANCEL
                        if(yf > 0.027273 && yf < 0.072727){
                            
                            if(navigation->state == quitWindow){
                                navigation->state= mainMenu;
                            }
                        }
                    }
                    

                    break;

                default:
                    break;
            }
        }
    }
    return flag;
}

