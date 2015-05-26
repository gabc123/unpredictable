//
//  up_menu.c
//  Unpredictable
//
//  Created by Zetterman on 2015-04-28.
//  Copyright (c) 2015 Zetterman. All rights reserved.
//  NOTERA, allt som inte står specifierat att det är gjort av någon annan är gjort av Joachim

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
#include "up_music.h"
#include "up_error.h"
#include "up_network_packet_utilities.h"

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
struct up_menu_button *up_generate_settings_button(int *numkey,struct up_key_map *keymap, struct up_vec3 pos,int hight,int width,struct up_vec3 textScale,float step)
{
    int count = 0;
    int i = 0;
    while(keymap[count].key != 0)
    {
        count++;
    }
    if (count <=0) {
        return NULL;
    }
    
    struct up_menu_button *buttonArray = malloc(sizeof(struct up_menu_button) * count);
    if (buttonArray == NULL)
    {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    struct up_mesh *mesh = up_mesh_menu_Botton();
    
    struct up_texture_data *textureButton = up_load_texture("button_keybind");
    if(textureButton == NULL){
        textureButton = up_load_texture("lala.png");
    }
    
    for (i = 0; i < count; i++) {
        strcpy(buttonArray[i].text ,SDL_GetKeyName(keymap[i].key));
        buttonArray[i].text_len = (int)strlen(buttonArray[i].text);
        buttonArray[i].textScale = textScale;
        
        buttonArray[i].pos = pos;
        pos.y -= step;
        buttonArray[i].width = width;
        buttonArray[i].hight = hight;
        
        buttonArray[i].mesh = mesh;
        buttonArray[i].tex = textureButton;
        
    }
    *numkey = count;
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
    
    struct up_vec3 text_pos = button->pos;
    text_pos.z -= 0.01;
    if (button->text_len > 2) {
        text_pos.x -= scale.x*button->text_len/10;
    }
    
    up_displayText(button->text,button->text_len,&text_pos,&button->textScale,fonts,shaderprog,0,color);
    
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
    registerMenu,
    usernameBar,
    quitWindow,
    settings,
    keyBindings,
    connecting,
    registering,
    shipSelect

};

enum loginBar_state
{
    usernameLogin,
    passwordLogin
};

enum registerBar_state
{
    usernameRegister,
    passwordRegister
};

enum soundEffect_state{
    
    soundOn,
    soundOff
};

enum music_state{
    
    musicOn,
    musicOff
};

enum ship_select{
    redShip,
    blueShip
    
};

struct navigationState{
    enum menu_states state;
    enum loginBar_state loginbar;
    enum registerBar_state registerbar;
    enum soundEffect_state toggleSound;
    enum music_state toogleMusic;
    enum ship_select shipSelect;
};

struct userData{
    char username[UP_LIMIT];
    char password[UP_LIMIT];
    int keypressUsername;
    int keypressPassword;
};

struct keybinding_state
{
    int change_key;
    int bind_key;
};

int up_keyBindingEvent(struct navigationState *navigation,struct up_key_map *keymap,struct up_menu_button *buttonArray,int numButtons,struct keybinding_state *bindstate);


int up_menuEventHandler(struct navigationState *navigation,
                        struct userData *user_data, struct soundLib *sound);


int up_menu(struct shader_module *shaderprog, struct soundLib *sound,struct up_key_map *keymap,struct up_font_assets *fonts,struct up_network_datapipe *network_connection){

    int status=1;
    
    //THEME MUSIC
    
    up_music(0, -1, sound);
    
    
    //KEYBINDINGS
    
    //BUTTONS
    int numKeyBindings = 0;
    struct up_vec3 keybind_pos = {0.23, 0.6 , 0.1};
    struct up_vec3 keytextscale = {0.025,0.025,0.025};
    
    struct up_vec3 keyButton_color = {0.0, 0.0, 0.0};

    struct up_menu_button *keybinding_buttonArray = up_generate_settings_button(&numKeyBindings, keymap, keybind_pos, 25, 100, keytextscale, 0.1);
    
    struct keybinding_state keybindState = {0};
    
    //DESCRIPTION
    
    struct up_vec3 keybindDescription_pos = {-0.1, 0.6 , 0.1};
    struct up_vec3 keyDescription_scale = {0.025,0.025,0.025};
    
    struct up_vec3 keyDescription_color = {1.0, 1.0, 1.0};
    
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
    struct up_texture_data *textureLoginOverlay = up_load_texture("registerUser.png");
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
        textureSettingsOverlay = up_load_texture("lala.png");
    }
    
    struct up_texture_data *textureKeybindingsOverlay = up_load_texture("keybindingsOverlay.png");
    if(textureKeybindingsOverlay == NULL){
        textureKeybindingsOverlay = up_load_texture("lala.png");
    }
    
    struct up_texture_data *textureConnectionStatus = up_load_texture("connecting.png");
    if(textureConnectionStatus == NULL){
        textureConnectionStatus = up_load_texture("lala.png");
    }
    
    struct up_texture_data *textureRegisterStatus = up_load_texture("register.png");
    if(textureRegisterStatus == NULL){
        textureRegisterStatus = up_load_texture("lala.png");
    }
    
    struct up_texture_data *textureShipSelection = up_load_texture("shipPic.png");
    if(textureShipSelection == NULL){
        textureShipSelection = up_load_texture("lala.png");
    }

    //TRANSFORMS
    up_matrix4_t transformLoginRegisterBottons;
    up_matrix4_t transformBackground;
    up_matrix4_t transformLoginOverlay;
    up_matrix4_t transformQuiteWindow;
    up_matrix4_t transformCogWheel;
    up_matrix4_t transformSettingsOverlay;
    up_matrix4_t transformKeybindingsOverlay;
    up_matrix4_t transformRegisterOverlay;
    
    up_matrix4_t transformConnectionSuccess;
    up_matrix4_t transformConnectionFalied;
    up_matrix4_t transformRegisterSuccess;
    up_matrix4_t transformRegisterFailed;
    
    up_matrix4_t transformShipBlue;
    up_matrix4_t transformShipRed;
    

    //TRANSLATION TRANSFORMS
    up_matrix4_t translationLoginRegisterButtons;
    up_matrix4_t translationLoginOverlay;
    up_matrix4_t translationQuiteWindow;
    up_matrix4_t translationCogWheel;
    up_matrix4_t translationSettingsOverlay;
    up_matrix4_t translationKeybindingsOverlay;
    up_matrix4_t translationRegisterOverlay;
    
    up_matrix4_t translationConnectionSuccess;
    up_matrix4_t translationConnectionFailed;
    up_matrix4_t translationRegisterSuccess;
    up_matrix4_t translationRegisterFailed;
    
    up_matrix4_t translationShipBlue;
    up_matrix4_t translationShipRed;
    
    //MESH LOADING
    struct up_mesh *background = up_meshMenuBackground();
    struct up_mesh *bottonLogin1 =up_meshBotton(0,0.95,0,0); //(float imageX, float imageY, float screenPosX, float screenPosY)
    struct up_mesh *bottonLogin2 =up_meshBotton(0,0.1,0,-0.25);
    struct up_mesh *overlay = up_meshLoginOverlay(0.84, 0.0);
    struct up_mesh *quiteWindow = up_meshQuitwindow();
    struct up_mesh *cogWheel = up_cogWheel();
    struct up_mesh *settingsOverlay = up_settingsOverlay();
    struct up_mesh *keybindingsOverlay = up_keybindingsOverlay();
    struct up_mesh *registerOverlay = up_meshLoginOverlay(1.0, 0.0);
    
    struct up_mesh *connectionSuccess = up_connectionOverlay(1.0, 0.5);
    struct up_mesh *connectionFailed= up_connectionOverlay(0.5, 0.0);
    struct up_mesh *registerSuccess = up_registerOverlay(1.0, 0.5);
    struct up_mesh *registerFailed = up_registerOverlay(0.5, 0.0);
    
    struct up_mesh *redShip = up_shipSelection(1.0, 0.0, 0.1, 0.2);
    struct up_mesh *blueShip = up_shipSelection(0.5, 0.0, -0.1, -0.5);

    //LOGIN AND REGISTER BUTTONS
    struct up_modelRepresentation scale1 ={{0,0,0},     //changes the scale of the bottons to x0.5
                                          {0,0,0},
                                          {0.5,0.5,0.5}};


    up_matrixModel(&translationLoginRegisterButtons, &scale1.pos, &scale1.rot, &scale1.scale);

    up_getModelViewPerspective(&transformLoginRegisterBottons, &translationLoginRegisterButtons, &identity, &identity);

    //BACKGROUND
    up_getModelViewPerspective(&transformBackground, &identity, &identity, &identity);

    //LOGIN/REGISTER OVERLAY
    struct up_modelRepresentation scale2 ={{0,0,0},     //no scale (yet)
                                           {0,0,0},
                                           {0.95,0.95,0.95}};

    
        //LOGIN
    up_matrixModel(&translationLoginOverlay, &scale2.pos, &scale2.rot, &scale2.scale);

    up_getModelViewPerspective(&transformLoginOverlay, &translationLoginOverlay, &identity, &identity);
    
        //REGISTER
    
    up_matrixModel(&translationRegisterOverlay, &scale2.pos, &scale2.rot, &scale2.scale);
    
    up_getModelViewPerspective(&transformRegisterOverlay, &translationRegisterOverlay, &identity, &identity);

    //QUIT WINDOW

    struct up_modelRepresentation scale3 ={{0,0,0},     //scaling
                                           {0,0,0},
                                           {0.7,0.7,0.7}};


    up_matrixModel(&translationQuiteWindow, &scale3.pos, &scale3.rot, &scale3.scale);

    up_getModelViewPerspective(&transformQuiteWindow, &translationQuiteWindow, &identity, &identity);

    
    

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
    
    
    //KEYBINDINGS OVERLAY
    
    struct up_modelRepresentation scale6={{0,0,0},     //scaling
                                         {0,0,0},
                                        {1,1,1}};
    
    up_matrixModel(&translationKeybindingsOverlay, &scale6.pos, &scale6.rot, &scale6.scale);
    
    up_getModelViewPerspective(&transformKeybindingsOverlay, &translationKeybindingsOverlay, &identity, &identity);

    //CONNECTING //REGISTER
    
    struct up_modelRepresentation scale7={{0,0,0},     //scaling
                                          {0,0,0},
                                          {0.6,0.7,0.6}};
    
            //Success
    up_matrixModel(&translationConnectionSuccess, &scale7.pos, &scale7.rot, &scale7.scale);
    
    up_getModelViewPerspective(&transformConnectionSuccess, &translationConnectionSuccess, &identity, &identity);
    
            //Failed
    up_matrixModel(&translationConnectionFailed, &scale7.pos, &scale7.rot, &scale7.scale);
    
    up_getModelViewPerspective(&transformConnectionFalied, &translationConnectionFailed, &identity, &identity);
    
            //success
    up_matrixModel(&translationRegisterSuccess, &scale7.pos, &scale7.rot, &scale7.scale);
    
    up_getModelViewPerspective(&transformRegisterSuccess, &translationRegisterSuccess, &identity, &identity);
    
            //failed
    up_matrixModel(&translationRegisterFailed, &scale7.pos, &scale7.rot, &scale7.scale);
    
    up_getModelViewPerspective(&transformRegisterFailed, &translationRegisterFailed, &identity, &identity);
    
    
    
    //SHIP SELECTION
    
    struct up_modelRepresentation scale9={{0,0,0},     //scaling
                                          {0,0,0},
                                          {1.0,1.0,1.0}};
    
    up_matrixModel(&translationShipBlue, &scale9.pos, &scale9.rot, &scale9.scale);
    
    up_getModelViewPerspective(&transformShipBlue, &translationShipBlue, &identity, &identity);
    
    up_matrixModel(&translationShipRed, &scale9.pos, &scale9.rot, &scale9.scale);
    
    up_getModelViewPerspective(&transformShipRed, &translationShipRed, &identity, &identity);
    

    
    //NAVIGATION
    struct navigationState navigation;

    navigation.state = mainMenu;
    navigation.loginbar = usernameLogin;
    navigation.registerbar = usernameRegister;
    navigation.toggleSound = soundOn;
    navigation.toogleMusic = musicOn;

    //FONT USERNAME/PASSWORD
    //struct up_font_assets *fonts = up_font_start_setup();
    struct up_vec3 textposusername = {-0.17, 0.045, 0};
    struct up_vec3 textscale = {0.025,0.025,0.025};
    
    struct up_vec3 textpospassword = {-0.17, -0.155, 0};

    //USER DATA
    struct userData user_data;
    user_data.keypressPassword=0;
    user_data.keypressUsername=0;

   // char *teststr1 = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNO";
   // char *teststr2 = "PQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¢";
    char *passwordstr = "**********************";
   // struct up_vec3 testtextpos1 = {-1.0, -0.50, 0};
   // struct up_vec3 testtextpos2 = {-1.0, -0.55, 0};

    
    int i = 0;  //used for loops
    int connectFlag = 0;
    int registerFlag = 0;
    
#define UP_ACCOUNT_DATA_MAX 5
    struct up_network_account_data accountData[UP_ACCOUNT_DATA_MAX];
    int packet_read = 0;
    
    // MENU LOOP
    while(status)
    {
        up_updateFrameTickRate();

        UP_renderBackground();                      //Clears the buffer and results an empty window.
        UP_shader_bind(shaderprog);                 //
        if (navigation.state != keyBindings) {
            status = up_menuEventHandler(&navigation, &user_data, sound);
        }else
        {
            status = up_keyBindingEvent(&navigation, keymap, keybinding_buttonArray, numKeyBindings,&keybindState);
        }
        
        // this function retrives data from server and store it into accountData array, pack_read is how many it filled
        // expande struct up_network_account_data with the type of information you need to get from network
        packet_read = up_network_getAccountData(accountData, UP_ACCOUNT_DATA_MAX, network_connection);
        
        
        
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

                
                up_displayText(user_data.username, user_data.keypressUsername, &textposusername, &textscale, fonts, shaderprog,0,NULL);
                   // up_displayText(teststr1, (int)strlen(teststr1), &testtextpos1, &textscale, fonts, shaderprog,0,NULL);
                    //up_displayText(teststr2, (int)strlen(teststr2), &testtextpos2, &textscale, fonts, shaderprog,0,NULL);
                
                up_displayText(passwordstr, user_data.keypressPassword, &textpospassword, &textscale, fonts, shaderprog,0,NULL);
            

                break;
                
            case registerMenu:
                
                UP_shader_update(shaderprog, &transformRegisterOverlay);
                up_texture_bind(textureLoginOverlay, 3);
                up_draw_mesh(registerOverlay);
                
                
                up_displayText(user_data.username, user_data.keypressUsername, &textposusername, &textscale, fonts, shaderprog,0,NULL);
                
                up_displayText(user_data.password, user_data.keypressPassword, &textpospassword, &textscale, fonts, shaderprog,0,NULL);
                
                
                break;
                
            case connecting:
                
                connectFlag=7;
                
                while (connectFlag == 0) { //waiting for server
                    
                    connectFlag = accountData->serverResponse;
                    
                }
                
                //CONNECTION SUCCESS
                if (connectFlag == LOGINSUCESS ) {
                    
                    
                    
                    
                    UP_shader_update(shaderprog, &transformConnectionSuccess);
                    up_texture_bind(textureConnectionStatus, 3);
                    up_draw_mesh(connectionSuccess);
                    
                    
                    navigation.state = shipSelect;
                    
                    //SDL_Delay(5000);
                
                }
                //CONNECTION FAILED
                else if (connectFlag == LOGINFAILED){
                    
                    UP_shader_update(shaderprog, &transformConnectionFalied);
                    up_texture_bind(textureConnectionStatus, 3);
                    up_draw_mesh(connectionFailed);
                    
                    //SDL_Delay(500);
                    
                    navigation.state = loginMenu;
                    
                }
                else {
                    printf("Connection flag error, not valid outcome \n");
                }
                
                break;
                
            case registering:
                

                up_network_registerAccount(user_data.username, user_data.password, UP_LIMIT, network_connection);
                
                
                while (registerFlag == 0) { //waiting for server
                    
                    registerFlag = accountData->serverResponse;

                }
                
                //REGISTERING SUCCESS
                if (registerFlag == REGSUCESSS ) {
                    
                    UP_shader_update(shaderprog, &transformRegisterSuccess);
                    up_texture_bind(textureRegisterStatus, 3);
                    up_draw_mesh(registerSuccess);
                    
                    //SDL_Delay(500);
                    
                    navigation.state = loginMenu;
                    
                    status=2;
                }
                //REGISTERING FAILED
                else if (registerFlag == REGFAILED){
                    
                    UP_shader_update(shaderprog, &transformRegisterFailed);
                    up_texture_bind(textureRegisterStatus, 3);
                    up_draw_mesh(registerFailed);
                    
                    //SDL_Delay(500);
                    
                    navigation.state = loginMenu;
                    registerFlag=0;
                    
                }
                else {
                    printf("Register flag error, not valid outcome \n");
                    
                }
                
                break;
                
            case shipSelect:
                
                UP_shader_update(shaderprog, &transformShipBlue);
                up_texture_bind(textureShipSelection, 2);
                up_draw_mesh(blueShip);
                
                UP_shader_update(shaderprog, &transformShipRed);
                up_texture_bind(textureShipSelection, 2);
                up_draw_mesh(redShip);
                
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
            case keyBindings:
                
                UP_shader_update(shaderprog, &translationKeybindingsOverlay);
                up_texture_bind(textureKeybindingsOverlay, 4);
                up_draw_mesh(keybindingsOverlay);
                
                for (i = 0; i < numKeyBindings; i++) {
                    up_drawbutton(shaderprog, &keybinding_buttonArray[i], fonts, &keyButton_color);
                    
                    
                    keybindDescription_pos= keybinding_buttonArray[i].pos;
                    keybindDescription_pos.x += -0.5;
                    
                    up_displayText(keymap[i].name, (unsigned int)strlen(keymap[i].name), &keybindDescription_pos , &keyDescription_scale, fonts, shaderprog, 0, &keyDescription_color);
                }
                
                break;
            default:
                break;
        }




        UP_openGLupdate();

    }

    up_generate_settings_freebuttons(keybinding_buttonArray);

    return status;
}

//int up_checkButtonClick(struct up_menu_button *button,int mouse_x,int mouse_y)

int up_keyBindingEvent(struct navigationState *navigation,struct up_key_map *keymap,struct up_menu_button *buttonArray,int numButtons,struct keybinding_state *bindstate)
{
    int flag = 1;
    SDL_Event event;
    int x = 0;
    int y = 0;
    int i = 0;
    
    
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flag = 0;
        }
        
        if(event.type == SDL_KEYUP)
        {
            // escape key, this will exit the keybinding state
            if (event.key.keysym.sym == 27) {
                navigation->state= settings;
                return flag;
            }
            
            if (bindstate->change_key == 1) {
                
                keymap[bindstate->bind_key].key = event.key.keysym.sym;
                strcpy(buttonArray[bindstate->bind_key].text ,SDL_GetKeyName(keymap[bindstate->bind_key].key));
                buttonArray[bindstate->bind_key].text_len = (int)strlen(buttonArray[bindstate->bind_key].text);
                bindstate->change_key = 0; // key set
            }
            
        }
        if(event.type == SDL_MOUSEBUTTONDOWN && (event.button.button == SDL_BUTTON_LEFT))
        {
            SDL_GetMouseState(&x, &y);
            for (i = 0; (i < numButtons) && (keymap[i].key != 0); i++)
            {
                if (up_checkButtonClick(&buttonArray[i], x, y)) {
                    bindstate->bind_key = i;
                    bindstate->change_key = 1;
                    //SDL_Delay(10); // so the button that is down rigth now is not used
                }
            }
        }
            
    }
    return flag;
}
int up_menuEventHandler(struct navigationState *navigation,
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
            
            //printf("%d\n", event.key.keysym.sym);
            
            
            
            
            
            
            if ((navigation->loginbar == usernameLogin) && (user_data->keypressUsername <= 30)) {

                //int data = event.key.keysym.sym;
                //printf("%d \n", data);

                if ((event.key.keysym.sym > 32) && ( event.key.keysym.sym < 127)) { // from a to z
                    user_data->username[user_data->keypressUsername] = event.key.keysym.sym;
                    user_data->keypressUsername++;
                }
                else if (event.key.keysym.sym == 32) {  //FOR TESTING, SPACE ERASES ALL
                    user_data->username[UP_LIMIT - 1] = '\0';
                    printf("%s\n",user_data->username);
                    user_data->keypressUsername = 0;
                }
                else if (event.key.keysym.sym == 8){  //BACKSPACE
                    if (user_data->keypressUsername != 0) {
                        user_data->keypressUsername = user_data->keypressUsername - 1;

                        user_data->username[user_data->keypressUsername] = '\0';
                    }
                }
                
            }
            
            
            if ((navigation->loginbar == passwordLogin) && (user_data->keypressPassword <= 30)) {
                
                //int data = event.key.keysym.sym;
                //printf("%d \n", data);
                
                if ((event.key.keysym.sym > 32) && ( event.key.keysym.sym < 127)) { // from a to z
                    user_data->password[user_data->keypressPassword] = event.key.keysym.sym;
                    user_data->keypressPassword++;
                }
                else if (event.key.keysym.sym == 32) {  //FOR TESTING, SPACE ERASES ALL
                    user_data->password[UP_LIMIT - 1] = '\0';
                    printf("%s\n",user_data->password);
                    user_data->keypressPassword = 0;
                }
                else if (event.key.keysym.sym == 8){  //BACKSPACE
                    if (user_data->keypressPassword != 0) {
                        user_data->keypressPassword = user_data->keypressPassword - 1;
                        
                        user_data->password[user_data->keypressPassword] = '\0';
                    }
                }

                
            }
            
            
            if (event.key.keysym.sym == 9){  //TAB
                
                
                if (navigation->state == loginMenu) {
                    if (navigation->loginbar == usernameLogin) {
                        navigation->loginbar = passwordLogin;
                    }
                    else if (navigation->loginbar == passwordLogin) {
                        navigation->loginbar = usernameLogin;
                    }
                }
                if (navigation->state==registerMenu) {
                    if (navigation->registerbar == usernameRegister) {
                        navigation->registerbar = passwordRegister;
                    }
                    else if (navigation->registerbar == passwordRegister) {
                        navigation->registerbar = usernameRegister;
                    }
                }

            }
            
            if (event.key.keysym.sym == 13){  //ENTER
                
                if (navigation->state == loginMenu) {
                    if (navigation->loginbar == usernameLogin) {
                        navigation->state = connecting;
                    }
                    else if (navigation->loginbar == passwordLogin) {
                        navigation->state = connecting;
                    }
                }
                if (navigation->state == registerMenu) {
                    if (navigation->registerbar == usernameRegister) {
                        navigation->state = registering;
                    }
                    else if (navigation->registerbar == passwordRegister) {
                        navigation->state = registering;
                    }
                }
            }
            
            if (event.key.keysym.sym == 27) { // ESC

                if (navigation->state == mainMenu) {
                    navigation->state= quitWindow;
                }

                else if (navigation->state == loginMenu) {
                    navigation->state= mainMenu;
                }
                
                else if (navigation->state == registerMenu) {
                    navigation->state= mainMenu;
                }
                
                else if (navigation->state == settings) {
                    navigation->state= mainMenu;
                }
                
                else if (navigation->state == keyBindings) {
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

                    //printf("X AND Y COORDINATES: %f %f , real x %d y %d\n", xf, yf,x,y); //test print
                    //INstead of checking the keypress first it should check the state of the navigation and then check the where the keypress where placed at
                    
                    if (navigation->state == shipSelect) {
                        
                        
                        
                        
                        
                        
                        
                    }
                    

                    //LOGIN BOTTON
                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of login screen
                        if(yf > 0.047727 && yf < 0.131818){

                            if(navigation->state == mainMenu){
                                 navigation->state = loginMenu;
                            }
                        }
                    }
                    
                    //LOGIN USERNAME BAR
                    
                    if(xf > -0.196875 && xf < 0.200000){        //coordinates of login screen
                        if(yf > 0.088636 && yf < 0.177273){
                            
                            if(navigation->state == loginMenu){
                                navigation->loginbar=usernameLogin;
                            }
                        }
                    }
                    
                    
                    //LOGIN PASSWORD BAR
                    
                    if(xf > -0.200000 && xf < 0.196875){        //coordinates of login screen
                        if(yf > -0.090909 && yf < -0.004545){
                            
                            if(navigation->state == loginMenu){
                                navigation->loginbar=passwordLogin;
                            }
                        }
                    }
                    

                    //REGISTER BOTTON
                    if(xf > -0.137500 && xf < 0.140625){        //coordinates of registration screen
                        if(yf > -0.068182 && yf < 0.015909){

                            if (navigation->state == mainMenu){
                                navigation->state=registerMenu;
                                
                                flag=2;
                            }
                        }
                    }

                    
                    //WRITE IN LOGIN BAR
                    if(xf > -0.198438 && xf < 0.195312){
                        if(yf > 0.093182 && yf < 0.175000){

                            if (navigation->state == loginMenu){
                                navigation->loginbar=usernameLogin;
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
                                
                                if (navigation->toggleSound == soundOn) {
                                    sound->toogleSoundEffects=0;
                                    navigation->toggleSound= soundOff;
                                    
                                    //printf("Sound OFF\n");
                                }
                                
                                else if(navigation->toggleSound == soundOff){
                                    sound->toogleSoundEffects=1;
                                    navigation->toggleSound= soundOn;
                                    
                                    up_music(1, 0, sound);
                                    
                                    //printf("Sound ON\n");
                                    
                                }
                                
                            }
                        }
                    }
                    
                    
                    if(xf > -0.135938 && xf < 0.132812){            //MUSIC
                        if(yf > 0.195455 && yf < 0.309091){
                            
                            if (navigation->state == settings) {
                                
                                if (navigation->toogleMusic == musicOn) {
                                    sound->toogleThemeMusic = 0;
                                    navigation->toogleMusic = musicOff;
                                    
                                    Mix_HaltChannel(0);
                                    
                                    //printf("music OFF\n");
                                }
                                else if (navigation->toogleMusic == musicOff){
                                    sound->toogleThemeMusic = 1;
                                    navigation->toogleMusic = musicOn;
                                    
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

