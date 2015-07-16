//
//  up_menu.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-14.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include <stdio.h>
#include "up_startmenu.h"
#include "up_ui_module.h"
#include "up_music.h"
#include "up_sdl_redirect.h"
#include "up_math.h"
#include "up_control_events.h"
#include "up_error.h"
#include "up_network_module.h"
#include "up_network_packet_utilities.h"
#include "up_menu_viewController.h"
#include "up_graphics_update.h"
#include "testmodels.h"

// adds next to the navigation stack and change the active menu
int navigation_nextMenu(struct up_menu_data *menu_data,enum up_menu_navigation next)
{
    if (menu_data->nav_stack_top > UP_MENU_NAVIGATION_MAXDEPTH - 1) {
        return 0;
    }
    *menu_data->activeInterface = next;
    menu_data->nav_stack_top++;
    menu_data->nav_stack[menu_data->nav_stack_top] = next;
    return 0;
}

// change the menu to the menu below in the nagvigation stac, if already at root then nothing
int navigation_prevMenu(struct up_menu_data *menu_data)
{
    int tmp_top = menu_data->nav_stack_top;
    menu_data->nav_stack_top = (tmp_top > 0) ? tmp_top - 1 : 0;
    *menu_data->activeInterface = menu_data->nav_stack[menu_data->nav_stack_top];
    return 0;
}

// replace the current menu without effecting the navigation stack
int navigation_replaceMenu(struct up_menu_data *menu_data,enum up_menu_navigation nav_menu)
{
    menu_data->nav_stack[menu_data->nav_stack_top] = nav_menu;
    *menu_data->activeInterface = nav_menu;
    return 0;
}

// clears all navigation locations, and set a new root menu
int navigation_resetMenu(struct up_menu_data *menu_data,enum up_menu_navigation nav_menu)
{
    menu_data->nav_stack_top = 0;
    menu_data->nav_stack[menu_data->nav_stack_top] = nav_menu;
    *menu_data->activeInterface = nav_menu;
    return 0;
}

// returns the navigation type of the menu below current
enum up_menu_navigation navigation_checkPrevMenu(struct up_menu_data *menu_data)
{
    int tmp_loc = (menu_data->nav_stack_top > 0) ? menu_data->nav_stack_top - 1 : 0;
    return menu_data->nav_stack[tmp_loc];
    
}


////// general navigation funcitons
int navigation_escapeKey(void *data)
{
    struct up_menu_data *state = (struct up_menu_data *)data;
    if (state == NULL) {
        return 0;
    }

    if (*state->activeInterface == startMenu) {
        navigation_nextMenu(state, quitMenu);
        return 0;
    }
    if (*state->activeInterface == connectingMenu) {
        state->netInfo.isActive = 0;
        state->netInfo.isSent = 0;
        state->netInfo.timeout = 0;
    }
    
    navigation_prevMenu(state);
    return 0;
}

////// general navigation textfield funcitons
int navigation_tabKey(void *data)
{
    struct up_menu_data *menu_data = (struct up_menu_data *)data;
    if (menu_data == NULL)  {
        return 0;
        
    }
    struct up_menu_interface *interface = &menu_data->interfaceArray[*menu_data->activeInterface];
    if ((interface == NULL) || (interface->numTextField <= 0)) {
        return 0;
    }
    
    switch (interface->active_textfield) {
        case noActiveTextField_menu: interface->active_textfield = usernameActive_menu; break;
        case usernameActive_menu: interface->active_textfield = passwordActive_menu; break;
        case passwordActive_menu: interface->active_textfield = usernameActive_menu; break;
        default:interface->active_textfield = noActiveTextField_menu; break;
    }
    return 0;
}

///// enter key

int navigation_enterKey(void *data)
{
    struct up_menu_data *menu_data = (struct up_menu_data *)data;
    if (menu_data == NULL)  {
        return 0;
        
    }
    enum up_menu_navigation nav = *menu_data->activeInterface;
    if ((nav != loginMenu) && (nav != registrateMenu)) {
        return 0;
    }
    
    //menu_data->
    menu_data->netInfo.isActive = 1;
    menu_data->netInfo.isSent = 0;
    menu_data->netInfo.timeout = SDL_GetTicks() + 10000;//10 seconds timeout
    navigation_nextMenu(menu_data, connectingMenu);
    
    return 0;
}

////// general navigation textfield funcitons

void user_data_backspace(char *strData,int *write_pos,struct up_ui_text *text)
{
    if (*write_pos <= 0) {
        return;
    }
    *write_pos = *write_pos - 1;
    strData[*write_pos] = '\0';
    text->length = *write_pos;
}

int navigation_backspaceKey(void *data)
{
    struct up_menu_data *menu_data = (struct up_menu_data *)data;
    if (menu_data == NULL)  {
        return 0;
        
    }
    struct up_menu_interface *interface = &menu_data->interfaceArray[*menu_data->activeInterface];
    if ( interface->numTextField <= 0) {
        return 0;
    }
    struct up_userLogin_data *userData = menu_data->user_data;
    switch (interface->active_textfield) {
        case usernameActive_menu:
            user_data_backspace(userData->name, &userData->writePos_name, interface->textFieldArray[0].elm->text);
            break;
        case passwordActive_menu:
            user_data_backspace(userData->password, &userData->writePos_pass, interface->textFieldArray[1].elm->text);
            break;
        default:
            break;
    }
    
    return 0;
}

static void user_data_write(char *strData,int *write_pos,struct up_ui_text *text,SDL_Keycode key)
{
    if (*write_pos >= text->max_size - 1) {
        return;
    }
    strData[*write_pos] = key;
    *write_pos = *write_pos + 1;
    strData[*write_pos] = '\0';
    text->length = *write_pos;
}

static int writeAccountInfo(struct up_userLogin_data *userData,struct up_menu_interface *interface,SDL_Keycode key)
{
    if (key <= 32 || 127 <= key) {
        return 0;
    }
    switch (interface->active_textfield) {
        case usernameActive_menu:
            user_data_write(userData->name, &userData->writePos_name, interface->textFieldArray[0].elm->text,key);
            break;
        case passwordActive_menu:
            user_data_write(userData->password, &userData->writePos_pass, interface->textFieldArray[1].elm->text,key);
            break;
        default:
            break;
    }
    return 0;
}


////////////////

int up_buttonPressed_keybind(int idx,void *data)
{
    struct up_keybinding_state *state = (struct up_keybinding_state*)data;
    if (state != NULL) {
        state->bind_key = idx;  //what key that should be changed
        state->change_key = 1;  // that a key has been selected
    }
    return 0;
}

void up_bindKey(struct up_key_map *keymap,struct up_ui_buttonArray *buttonArray,struct up_keybinding_state *bindstate,SDL_Keycode key)
{
    int idx = bindstate->bind_key;
    char *str = NULL;
    struct up_ui_text *text = NULL;
    if (bindstate->change_key == 1) {
        keymap[idx].key = key;
        // get the ui text element of the button
        text = buttonArray[idx].elm->text;
        
        // change the button text to the new key
        str = text->text;
        strncpy(str, SDL_GetKeyName(key), UP_BUTTON_KEYNAME_MAX);
        text->length = (int)strlen(str);
        bindstate->change_key = 0; // key set
    }
}


int up_buttonPressed_toggleMusic(int idx,void *data)
{
    struct up_menu_data *state = (struct up_menu_data *)data;
    if (state == NULL) {
        return 0;
    }
    
    struct soundLib *sound = state->sound;
    if (sound->toogleThemeMusic == 1) {
        sound->toogleThemeMusic = 0;
        Mix_HaltChannel(0);
    }else
    {
        sound->toogleThemeMusic = 1;
        up_music(0, -1, sound); ///starts music on infinit loop
    }
    return 0;
}

int up_buttonPressed_toggleSound(int idx,void *data)
{
    struct up_menu_data *state = (struct up_menu_data *)data;
    if (state == NULL) {
        return 0;
    }
    struct soundLib *sound = state->sound;
    if (sound->toogleSoundEffects == 1) {
        sound->toogleSoundEffects = 0;
        
    }else
    {
        sound->toogleSoundEffects = 1;
        up_music(1, 0, sound);  //track 1, plat one time,
    }
    return 0;
}

int up_buttonPressed_keySettings(int idx,void *data)
{
    struct up_menu_data *state = (struct up_menu_data *)data;
    if (state != NULL) {
        navigation_nextMenu(state, keyBindMenu);
    }
    
    return 0;
}

//////////// registartion, and so on

int textfield_selected(int idx,void *data)
{
    struct up_menu_interface *interface = (struct up_menu_interface *)data;
    if (interface == NULL) {
        return 0;
        
    }
    
    switch (idx) {
        case 0: interface->active_textfield = usernameActive_menu; break;
        case 1: interface->active_textfield = passwordActive_menu; break;
        default:interface->active_textfield = noActiveTextField_menu; break;
    }
    return 0;
}

//////////// main menu
static void update_textField(struct up_userLogin_data *userData,struct up_ui_textFieldArray *textFieldArray)
{
    struct up_ui_text *text = textFieldArray[0].elm->text;
    text->length = (userData->writePos_name < text->max_size) ? userData->writePos_name : text->max_size;
    text = textFieldArray[1].elm->text;
    text->length = (userData->writePos_pass < text->max_size) ? userData->writePos_pass : text->max_size;
}

int up_interface_account_update(struct up_menu_data *menu_data)
{
    if (menu_data == NULL)  {
        return 0;
        
    }
    struct up_menu_interface *interface = &menu_data->interfaceArray[*menu_data->activeInterface];
    if ( interface->numTextField <= 0) {
        return 0;
    }
    if (interface->active_textfield != noActiveTextField_menu) {
        update_textField(menu_data->user_data, interface->textFieldArray);
    }
    return 0;
}

int up_menu_mainNavigation(int idx,void *data)
{
    struct up_menu_data *menu_data = (struct up_menu_data *)data;
    if (menu_data != NULL) {
        switch (idx) {
            case 0: *menu_data->menuExitFlags = 2; break;
            case 1: navigation_nextMenu(menu_data, loginMenu); break;
            case 2: navigation_nextMenu(menu_data, registrateMenu); break;
            case 3: navigation_nextMenu(menu_data, settingsMenu); break;
            default:
                return 0;
                break;
        }
    }
    up_interface_account_update(menu_data);
    return 0;
}

////////////////////// event handlers

//Magnus
int up_menu_event(struct up_menu_interface *interface,struct up_menu_data *menu_data,struct up_key_map *keymap,struct up_keybinding_state *bindstate)
{
    int flag = 1;
    SDL_Event event;
    int x = 0;
    int y = 0;
    SDL_Keycode key;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            flag = 0;
            *menu_data->menuExitFlags = -1;
        }
        // faster response time if we write on down, instead of up
        if (event.type == SDL_KEYDOWN) {
            key = event.key.keysym.sym;
            if (interface->active_textfield != noActiveTextField_menu) {
                writeAccountInfo(menu_data->user_data, interface, key);
            }
        }
        
        if(event.type == SDL_KEYUP)
        {
            key = event.key.keysym.sym;
            up_ui_controller_updateKeyAction(interface->controller_keyInput, key);
            if (bindstate->change_key == 1) {
                up_bindKey(keymap, interface->buttonArray, bindstate, key);
            }
            
        }
        if(event.type == SDL_MOUSEBUTTONDOWN && (event.button.button == SDL_BUTTON_LEFT))
        {
            SDL_GetMouseState(&x, &y);
            up_ui_controller_updateClick(interface->controller_glue, x, y);
        }
        
    }
    return flag;
}

static int up_menu_network_sendAccountRequset(struct up_menu_data *menu_data)
{
    enum up_menu_navigation connection_type = navigation_checkPrevMenu(menu_data);
    struct up_userLogin_data *user_data = menu_data->user_data;
    switch (connection_type) {
        case loginMenu:
            up_network_loginAccount(user_data->name, user_data->password, user_data->writePos_pass, menu_data->network_connection);
            break;
        case registrateMenu:
            up_network_registerAccount(user_data->name, user_data->password, user_data->writePos_pass, menu_data->network_connection);
            break;
        default:
            break;
    }
    return 0;
}

int up_menu_network_toServer(struct up_menu_data *menu_data)
{
    if (*menu_data->activeInterface != connectingMenu) {return 0;}
    if (menu_data->netInfo.isActive == 0) {return 0;}
    struct up_menu_network_info *netinfo = &menu_data->netInfo;
    unsigned int currentTick = SDL_GetTicks();
    unsigned int elapsed_time = currentTick - netinfo->send_timestamp;
    if (netinfo->isSent && elapsed_time < 1000 ) {
        return 0;
    }
    
    if (currentTick > netinfo->timeout) {
        netinfo->isActive = 0;
        netinfo->isSent = 0;
        navigation_prevMenu(menu_data);
    }
    
    netinfo->send_timestamp = currentTick;
    up_menu_network_sendAccountRequset(menu_data);
    netinfo->isSent = 1;
    return 0;
}

int up_menu_network_fromServer(struct up_menu_data *menu_data)
{
    if (*menu_data->activeInterface != connectingMenu) {return 0;}
    if (menu_data->netInfo.isActive == 0) {return 0;}
    up_network_getAccountData(menu_data->accountData, 1, menu_data->network_connection);
    menu_data->netInfo.response = menu_data->accountData->serverResponse;
    switch (menu_data->netInfo.response) {
        case REGSUCESSS :
            navigation_resetMenu(menu_data, startMenu);
            break;
        case LOGINSUCESS :
            navigation_resetMenu(menu_data, startMenu);
            *menu_data->menuExitFlags = 2;
            break;
        case REGFAILED :
            navigation_prevMenu(menu_data);
            break;
        case LOGINFAILED :
            navigation_prevMenu(menu_data);
            break;
            
        default:
            return 0;
            break;
    }
    menu_data->netInfo.isActive = 0;
    menu_data->netInfo.isSent = 0;
    menu_data->netInfo.timeout = 0;
    return 0;
}

// render menu

void up_menu_renderInferface(struct up_menu_interface *interface,struct up_shader_module *shader_prog)
{
    up_ui_rectArray_render(interface->rectArray, interface->numRect, shader_prog);
    up_ui_textArray_render(interface->textArray, interface->numText, shader_prog);
    up_ui_textFieldArray_render(interface->textFieldArray, interface->numTextField, shader_prog);
    up_ui_buttonArray_render(interface->buttonArray, interface->numButtons, shader_prog);
    
}

int up_startmenu(struct up_map_data *mapData,struct up_key_map *keymap,struct up_network_datapipe *connection,struct soundLib *sound,struct up_font_assets *fonts,struct up_shader_module *shader)
{
    struct up_menu_interface interfaceArray[navigationMenu_count];
    struct up_menu_interface interfaceZero = {0};
    int i = 0;
    for (i = 0; i < navigationMenu_count; i++) {
        interfaceArray[i] = interfaceZero;
    }
    struct up_menu_data menu_data = {0};
    enum up_menu_navigation navigation = startMenu;;
    struct up_userLogin_data userData = {0};
    
    struct up_network_account_data accountData[1] = {0};
    
    int exitFlag = 0;
    menu_data.menuExitFlags = &exitFlag;
    
    menu_data.activeInterface = &navigation;
    // sets the navigation stack
    navigation_resetMenu(&menu_data, navigation);
    
    menu_data.interfaceArray = &interfaceArray[0];
    menu_data.numInterface = navigationMenu_count;
    
    menu_data.user_data = &userData;
    menu_data.mapData = mapData;
    menu_data.sound = sound;
    menu_data.accountData = accountData;
    menu_data.network_connection = connection;
    
    struct up_keybinding_state keybind_state = {0};
    struct up_vec3 zero = {0};
    struct up_ui_clickArea area = {0};
    struct up_vec3 pos = {0.0, 0.0, 0.4};
    struct up_vec3 textScale = {0.025, 0.025, 0.025};
    area.hight = 25;
    area.width = 250;
    area.pos = pos;
    

    up_menu_interface_main_new(&interfaceArray[startMenu], &menu_data, area, textScale, fonts);
    up_menu_interface_settings_new(&interfaceArray[settingsMenu], &menu_data, area, textScale, fonts);
    up_menu_interface_account_new(&interfaceArray[registrateMenu], "Registrate", &menu_data, area, textScale, fonts);
    up_menu_interface_account_new(&interfaceArray[loginMenu], "Login", &menu_data, area, textScale, fonts);
    up_menu_interface_connecting_new(&interfaceArray[connectingMenu], &menu_data, area, textScale, fonts);
    area.pos.y = 0.5;
    up_menu_interface_keyBinding_new(&interfaceArray[keyBindMenu], keymap, area, textScale, 0.1, &keybind_state, fonts);
    
    // bind key to action
    struct up_ui_controller_keyInput nav_keyInput[] = {
    {SDLK_ESCAPE,navigation_escapeKey,&menu_data},
    0,NULL,NULL};
    interfaceArray[startMenu].controller_keyInput = nav_keyInput;
    interfaceArray[quitMenu].controller_keyInput = nav_keyInput;
    interfaceArray[settingsMenu].controller_keyInput = nav_keyInput;
    interfaceArray[keyBindMenu].controller_keyInput = nav_keyInput;
    interfaceArray[connectingMenu].controller_keyInput = nav_keyInput;
    
    // bind key to action
    struct up_ui_controller_keyInput nav_account_keyInput[] = {
        {SDLK_BACKSPACE,navigation_backspaceKey,&menu_data},
        {SDLK_ESCAPE,navigation_escapeKey,&menu_data},
        {SDLK_TAB,navigation_tabKey,&menu_data},
        {SDLK_RETURN,navigation_enterKey,&menu_data},
        0,NULL,NULL};
    
    interfaceArray[registrateMenu].controller_keyInput = nav_account_keyInput;
    interfaceArray[loginMenu].controller_keyInput = nav_account_keyInput;


    struct up_texture *backgroundTexture = up_load_texture("1971687");
    struct up_mesh *background = up_mesh_menu_Botton();
    up_matrix4_t identity = up_matrix4identity();
    up_matrix4_t backgroundModel = identity;
    struct up_vec3 backPos = {0.0, 0.0, 0.5};
    struct up_vec3 backScale = {1.0, 1.0, 1.0};
    up_matrixModel(&backgroundModel, &backPos, &zero, &backScale);
    
    while (exitFlag == 0) {
        UP_renderBackground();
        up_shader_bind(shader);
        
        up_menu_event(&interfaceArray[navigation], &menu_data, keymap, &keybind_state);
        
        // network comunications
        up_menu_network_toServer(&menu_data);
        up_menu_network_fromServer(&menu_data);
        
        // background rendering
        up_texture_bind(backgroundTexture, 0);
        up_shader_update(shader, &backgroundModel);
        up_mesh_draw(background);
        
        up_menu_renderInferface(&interfaceArray[navigation], shader);
        UP_openGLupdate();
    }
    up_menu_interface_main_free(&interfaceArray[startMenu]);
    up_menu_interface_settings_free(&interfaceArray[settingsMenu]);
    up_menu_interface_keyBinding_free(&interfaceArray[keyBindMenu]);
    up_menu_interface_account_free(&interfaceArray[registrateMenu]);
    up_menu_interface_account_free(&interfaceArray[loginMenu]);
    
    return exitFlag;
}

