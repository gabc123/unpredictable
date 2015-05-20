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
#include "up_error.h"

#define UP_LIMIT 30


#include "up_sdl_redirect.h"  //mouse event handlingr


// magnus
struct up_menu_button
{
    struct up_vec3 pos;
    int width;
    int hight;
    
    char text[30];
    int text_len;
    struct up_vec3 textScale;
    struct up_mesh *mesh;
    struct up_texture_data *tex;
};
#define UP_SCREEN_WIDTH 1280
#define UP_SCREEN_HIGHT 800

// magnus
struct up_menu_button *up_generate_settings_button(int *numkey,struct up_key_map *keymap, struct up_vec3 pos,int hight,int width,struct up_vec3 textScale)
{
    int count = 0;
    int i = 0;
    while(keymap[count].key != 0)
    {
        count++;
    }
    
    struct up_menu_button *buttonArray = malloc(sizeof(struct up_menu_button) * count);
    if (buttonArray == NULL)
    {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    struct up_mesh *mesh = up_mesh_menu_Botton();
    
    struct up_texture_data *textureButton = up_load_texture("placeholder.png");
    if(textureButton == NULL){
        textureButton = up_loadImage_withAlpha("lala.png");
    }
    
    for (i = 0; i < count; i++) {
        strcpy(buttonArray[i].text ,SDL_GetKeyName(keymap[i].key));
        buttonArray[i].text_len = (int)strlen(buttonArray[i].text);
        buttonArray[i].textScale = textScale;
        
        buttonArray[i].pos = pos;
        pos.y -= 0.02;
        buttonArray[i].width = width;
        buttonArray[i].hight = hight;
        
        buttonArray[i].mesh = mesh;
        buttonArray[i].tex = textureButton;
        
    }
    return buttonArray;
}

// magnus
void up_generate_settings_freebuttons(struct up_menu_button *buttonArray)
{
    free(buttonArray);
}

// magnus
void up_drawbutton(struct shader_module *shaderprog,struct up_menu_button *button,struct up_font_assets *fonts,struct up_vec3 *color)
{
    up_matrix4_t modelMatrix;
    struct up_vec3 rot = {0};
    struct up_vec3 scale;
    scale.x = (float)button->width/UP_SCREEN_WIDTH;
    scale.y = (float)button->hight/UP_SCREEN_HIGHT;
    scale.z = 1;
    
    up_matrixModel(&modelMatrix,&button->pos,&rot,&scale);
    
    UP_shader_update(shaderprog,&modelMatrix);
    up_texture_bind(button->tex, 2);
    up_draw_mesh(button->mesh);
    
    up_displayText(button->text,button->text_len,&button->pos,&button->textScale,fonts,shaderprog,0,color);
    
}

// magnus
int up_checkButtonClick(struct up_menu_button *button,int mouse_x,int mouse_y)
{
    int x = UP_SCREEN_WIDTH*(button->pos.x + 1.0)/2;
    int y = UP_SCREEN_HIGHT - UP_SCREEN_HIGHT*(button->pos.y + 1.0)/2;
    int width = button->width;
    int hight = button->hight;
    x = x - width/2; // the xy coord is the center of the button
    y = y - hight/2; // the xy coord is the center of the button
    
    // check if the click is inside the buttom, returns "true" elese "false"
    return ((x <= mouse_x) && ( mouse_x <= x + width) && (y <= mouse_y) && ( mouse_y <= y + hight));
}


enum menu_states
{
    mainMenu,
    loginMenu,
    usernameBar,
    quitWindow,
    settings,
    keyBindings

};

enum loginBar_state
{
    writeOn,
    writeOff
};

enum soundEffect_state{
    
    soundOn,
    soundOff
};

enum music_state{
    
    musicOn,
    musicOff
};

struct navigationState{
    enum menu_states state;
    enum loginBar_state status;
    enum soundEffect_state toggle;
    enum music_state toogle2;
};

struct userData{
    char username[UP_LIMIT];
    char password[UP_LIMIT];
    int keypress;
};


int up_menuEventHandler(struct navigationState *navigation, struct navigationState *loginBar,
                        struct navigationState *soundToggle, struct navigationState *musicToogle,
                        struct userData *user_data, struct soundLib *sound);


int up_menu(struct shader_module *shaderprog, struct soundLib *sound){

    int status=1;
    
    //THEME MUSIC
    
    up_music(0, -1, sound);
    
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
    
    struct up_texture_data *textureCogWheel = up_load_texture("cogWheel.png");
    if (textureCogWheel==NULL) {
        textureCogWheel = up_load_texture("lala.png");
    }
    
    struct up_texture_data *textureSettingsOverlay = up_load_texture("settings.png");
    if(textureSettingsOverlay == NULL){
        textureSettingsOverlay = up_loadImage_withAlpha("lala.png");
    }
    

    //TRANSFORMS
    up_matrix4_t transformLoginRegisterBottons;
    up_matrix4_t transformBackground;
    up_matrix4_t transformLoginOverlay;
    up_matrix4_t transformQuiteWindow;
    up_matrix4_t transformCogWheel;
    up_matrix4_t transformSettingsOverlay;

    //TRANSLATION TRANSFORMS
    up_matrix4_t translationLoginRegisterButtons;
    up_matrix4_t translationLoginOverlay;
    up_matrix4_t translationQuiteWindow;
    up_matrix4_t translationCogWheel;
    up_matrix4_t translationSettingsOverlay;

    //MESH LOADING
    struct up_mesh *background = up_meshMenuBackground();
    struct up_mesh *bottonLogin1 =up_meshBotton(0,0.95,0,0); //(float imageX, float imageY, float screenPosX, float screenPosY)
    struct up_mesh *bottonLogin2 =up_meshBotton(0,0.1,0,-0.25);
    struct up_mesh *overlay = up_meshLoginOverlay();
    struct up_mesh *quiteWindow = up_meshQuitwindow();
    struct up_mesh *cogWheel = up_cogWheel();
    struct up_mesh *settingsOverlay = up_settingsOverlay();
    
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

    //COGWHEEL
    
    struct up_modelRepresentation scale4 ={{0,0,0},     //scaling
                                           {0,0,0},
                                           {0.4,0.4,0.4}};

    up_matrixModel(&translationCogWheel, &scale4.pos, &scale4.rot, &scale4.scale);
    
    up_getModelViewPerspective(&transformCogWheel, &translationCogWheel, &identity, &identity);
    
    
    //SETTINGS OVERLAY
    
    struct up_modelRepresentation scale5={{0,0,0},     //scaling
                                          {0,0,0},
                                          {1,1,1}};

    up_matrixModel(&translationSettingsOverlay, &scale5.pos, &scale5.rot, &scale5.scale);
    
    up_getModelViewPerspective(&transformSettingsOverlay, &translationSettingsOverlay, &identity, &identity);
    
    //NAVIGATION
    struct navigationState navigation;
    struct navigationState loginBar;
    struct navigationState soundToggle;
    struct navigationState musicToggle;

    navigation.state = mainMenu;
    loginBar.status = writeOff;
    soundToggle.toggle = soundOn;
    musicToggle.toogle2 = musicOn;
    


    //USER DATA
    struct userData user_data;
    user_data.keypress=0;

    char *teststr1 = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNO";
    char *teststr2 = "PQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¢";
    struct up_vec3 testtextpos1 = {-1.0, -0.50, 0};
    struct up_vec3 testtextpos2 = {-1.0, -0.55, 0};

    
    
    
    
    // MENU LOOP
    while(status)
    {
        up_updateFrameTickRate();

        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //

        status = up_menuEventHandler(&navigation, &loginBar, &soundToggle, &musicToggle, &user_data, sound);
        
        //STATUS FLAG FOR MAIN GAME LOOP
        if (status==2) {
            break;
        }
        
        //BACKGROUND
        UP_shader_update(shaderprog,&transformBackground);
        up_texture_bind(textureMenuBackground, 1);
        up_draw_mesh(background);


        switch (navigation.state) {
            case mainMenu:
                UP_shader_update(shaderprog,&transformLoginRegisterBottons);     //button1
                up_texture_bind(textureBottonLogin, 2);
                up_draw_mesh(bottonLogin1);

                UP_shader_update(shaderprog,&transformLoginRegisterBottons);     //button2
                up_texture_bind(textureBottonLogin, 2);
                up_draw_mesh(bottonLogin2);
                
                UP_shader_update(shaderprog,&translationCogWheel);     //settings
                up_texture_bind(textureCogWheel, 3);
                up_draw_mesh(cogWheel);
                
                break;

            case loginMenu:

                UP_shader_update(shaderprog, &transformLoginOverlay);
                up_texture_bind(textureLoginOverlay, 3);
                up_draw_mesh(overlay);


                if (navigation.status == writeOn) {
                    up_displayText(user_data.username, user_data.keypress, &textpos, &textscale, fonts, shaderprog,0,NULL);
                    up_displayText(teststr1, (int)strlen(teststr1), &testtextpos1, &textscale, fonts, shaderprog,0,NULL);
                    up_displayText(teststr2, (int)strlen(teststr2), &testtextpos2, &textscale, fonts, shaderprog,0,NULL);

                }


                break;

            case quitWindow:

                UP_shader_update(shaderprog, &transformQuiteWindow);
                up_texture_bind(textureQuiteWindow, 2);
                up_draw_mesh(quiteWindow);

                break;
                
            case settings:
                
                UP_shader_update(shaderprog,&translationCogWheel);     //settings
                up_texture_bind(textureCogWheel, 3);
                up_draw_mesh(cogWheel);
                
                UP_shader_update(shaderprog, &translationSettingsOverlay);
                up_texture_bind(textureSettingsOverlay, 3);
                up_draw_mesh(settingsOverlay);
                
                break;
                
            default:
                break;
        }




        UP_openGLupdate();

    }



    return status;
}


int up_menuEventHandler(struct navigationState *navigation, struct navigationState *loginBar,
                        struct navigationState *soundToggle, struct navigationState *musicToogle,
                        struct userData *user_data, struct soundLib *sound)
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
                
                if (navigation->state == settings) {
                    navigation->state= mainMenu;
                }
                
                if (navigation->state == keyBindings) {
                    navigation->state= settings;
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

                    printf("X AND Y COORDINATES: %f %f , real x %d y %d\n", xf, yf,x,y); //test print


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
                    
                    
                    //COGWHEEL BUTTON
                    
                    if(xf > 0.600000 && xf < 0.837500){
                        if (yf < -0.502273 && yf > -0.675000){
                            
                            if (navigation->state == mainMenu) {
                                navigation->state= settings;
                            }
                            
                            else if (navigation->state == settings) {
                                navigation->state= mainMenu;
                            }
                        }
                    }

                    //SETTINGS OVERLAY
                    
                    if(xf > -0.135938 && xf < 0.132812){            //SOUND EFFECTS
                        if(yf > 0.034091 && yf < 0.145455){
                            
                            if(navigation->state == settings){
                                
                                if (soundToggle->toggle == soundOn) {
                                    sound->toogleSoundEffects=0;
                                    soundToggle->toggle= soundOff;
                                    
                                    //printf("Sound OFF\n");
                                }
                                
                                else if(soundToggle->toggle == soundOff){
                                    sound->toogleSoundEffects=1;
                                    soundToggle->toggle= soundOn;
                                    
                                    up_music(1, 0, sound);
                                    
                                    //printf("Sound ON\n");
                                    
                                }
                                
                            }
                        }
                    }
                    
                    
                    if(xf > -0.135938 && xf < 0.132812){            //MUSIC
                        if(yf > 0.195455 && yf < 0.309091){
                            
                            if (navigation->state == settings) {
                                
                                if (musicToogle->toogle2 == musicOn) {
                                    sound->toogleThemeMusic = 0;
                                    musicToogle->toogle2 = musicOff;
                                    
                                    Mix_HaltChannel(0);
                                    
                                    //printf("music OFF\n");
                                }
                                else if (musicToogle->toogle2 == musicOff){
                                    sound->toogleThemeMusic = 1;
                                    musicToogle->toogle2 = musicOn;
                                    
                                    up_music(0, -1, sound);
                                    
                                    //printf("music ON\n");
                                }
                            }
                        }
                    }
                    
                    if(xf > -0.135938 && xf < 0.132812){            //SETTINGS
                        if(yf > -0.143182 && yf < -0.020455){
                            
                            if (navigation->state == settings) {
                                
                                navigation->state = keyBindings;
                                
                                //do stuff

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

