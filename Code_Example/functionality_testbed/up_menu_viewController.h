//
//  up_menu_viewController.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-16.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_MENU_VIEWCONTROLLER_H
#define UP_MENU_VIEWCONTROLLER_H

#define UP_NAME_MAX_LENGTH 30

#include "up_sdl_redirect.h"
#include "up_ui_module.h"
#include "up_control_events.h"

struct up_userLogin_data
{
    char name[UP_NAME_MAX_LENGTH];
    char password[UP_NAME_MAX_LENGTH];
    int writePos_name;
    int writePos_pass;
};

enum up_menu_activeTextfield
{
    noActiveTextField_menu = 0,
    usernameActive_menu = 1,
    passwordActive_menu = 2
};

//magnus
struct up_menu_interface
{
    struct up_ui_buttonArray *buttonArray;
    struct up_ui_textArray *textArray;
    struct up_ui_textFieldArray *textFieldArray;
    struct up_ui_rectArray *rectArray;
    
    int numButtons;
    int numText;
    int numTextField;
    int numRect;
    
    enum up_menu_activeTextfield active_textfield;   // 0 = none, 1 = login,
    
    struct up_ui_controller_glue *controller_glue;
    struct up_ui_controller_keyInput *controller_keyInput;
};

enum up_menu_navigation
{
    startMenu = 0,
    settingsMenu,
    keyBindMenu,
    registrateMenu,
    loginMenu,
    quitMenu,
    navigationMenu_count
};

struct up_menu_data
{
    int *menuExitFlags;
    enum up_menu_navigation *activeInterface;
    struct up_menu_interface *interfaceArray;
    int numInterface;
    struct soundLib *sound;
    struct up_userLogin_data *user_data;
    struct up_map_data *mapData;
    struct up_network_datapipe *network_connection;
};

#define UP_BUTTON_KEYNAME_MAX 20

// key bindings
struct up_keybinding_state
{
    int change_key;
    int bind_key;
};

///////// functions




////// general navigation funcitons
int navigation_escapeKey(void *data);
////// general navigation textfield funcitons
int navigation_tabKey(void *data);
////// general navigation textfield funcitons

void user_data_backspace(char *strData,int *write_pos,struct up_ui_text *text);
int navigation_backspaceKey(void *data);

////

//magnus

int up_buttonPressed_keybind(int idx,void *data);

void up_bindKey(struct up_key_map *keymap,struct up_ui_buttonArray *buttonArray,struct up_keybinding_state *bindstate,SDL_Keycode key);

int up_buttonPressed_toggleMusic(int idx,void *data);
int up_buttonPressed_toggleSound(int idx,void *data);
int up_buttonPressed_keySettings(int idx,void *data);

int textfield_selected(int idx,void *data);

int up_menu_mainNavigation(int idx,void *data);



////// creates the diffrent menues

struct up_menu_interface *up_menu_interface_keyBinding_new(struct up_menu_interface *interface,
                                                           struct up_key_map *keymap,
                                                           struct up_ui_clickArea clickArea,
                                                           struct up_vec3 textScale,float step,
                                                           struct up_keybinding_state *state_data,
                                                           struct up_font_assets *fonts);
void up_menu_interface_keyBinding_free(struct up_menu_interface *interface);

struct up_menu_interface *up_menu_interface_settings_new(struct up_menu_interface *interface,
                                                         struct up_menu_data *menu_data,
                                                         struct up_ui_clickArea clickArea,
                                                         struct up_vec3 textScale,
                                                         struct up_font_assets *fonts);
void up_menu_interface_settings_free(struct up_menu_interface *interface);

//up_ui_clickArea
struct up_menu_interface *up_menu_interface_account_new(struct up_menu_interface *interface,
                                                        char *title,
                                                        struct up_menu_data *menu_data,
                                                        struct up_ui_clickArea clickArea,
                                                        struct up_vec3 textScale,
                                                        struct up_font_assets *fonts);
void up_menu_interface_account_free(struct up_menu_interface *interface);
//////////// main menu


struct up_menu_interface *up_menu_interface_main_new(struct up_menu_interface *interface,
                                                     struct up_menu_data *menu_data,
                                                     struct up_ui_clickArea clickArea,
                                                     struct up_vec3 textScale,
                                                     struct up_font_assets *fonts);
void up_menu_interface_main_free(struct up_menu_interface *interface);






#endif /* defined(__unpredictable_xcode__up_menu_viewController__) */
