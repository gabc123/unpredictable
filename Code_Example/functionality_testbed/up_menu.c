//
//  up_menu.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-14.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include <stdio.h>
#include "up_ui_module.h"
#include "up_music.h"
#include "up_sdl_redirect.h"
#include "up_math.h"
#include "up_control_events.h"
#include "up_error.h"
#include "up_network_module.h"


#define UP_NAME_MAX_LENGTH 30

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
    startMenu,
    settingsMenu,
    keyBindMenu,
    registrateMenu,
    loginMenu,
    quitMenu
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

////// general navigation funcitons
static int navigation_escapeKey(void *data)
{
    struct up_menu_data *state = (struct up_menu_data *)data;
    if (state == NULL) {
        return 0;
    }
    
    switch (*state->activeInterface) {
        case startMenu: *state->activeInterface = quitMenu; break;
        case keyBindMenu: *state->activeInterface = settingsMenu; break;
        default:*state->activeInterface = startMenu; break;
    }
    
    return 0;
}

////// general navigation textfield funcitons
static int navigation_tabKey(void *data)
{
    struct up_menu_interface *interface = (struct up_menu_interface *)data;
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

////// general navigation textfield funcitons

static void user_data_backspace(char *strData,int *write_pos,struct up_ui_text *text)
{
    if (*write_pos <= 0) {
        return;
    }
    *write_pos = *write_pos - 1;
    strData[*write_pos] = '\0';
    text->length = *write_pos;
}

static int navigation_backspaceKey(void *data)
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
            user_data_backspace(userData->name, &userData->writePos_name, interface->textFieldArray[0].elm->text);
            break;
        default:
            break;
    }
    
    return 0;
}


////

//magnus
static struct up_menu_interface *up_menu_interface_newStorage(struct up_menu_interface *interface,int numButtons,int numText,int numTextFields,int numRect)
{
    if (interface == NULL) {
        UP_ERROR_MSG("Passed in interface == NULL");
        return NULL;
    }
    
    interface->controller_glue = NULL;
    interface->controller_keyInput = NULL;
    
    struct up_ui_buttonArray *buttonArray = NULL;
    if (numButtons > 0) {
        buttonArray = malloc(sizeof(struct up_ui_buttonArray)*numButtons);
        if (buttonArray == NULL)
        {
            UP_ERROR_MSG("malloc failed");
            return NULL;
        }
    }
    interface->buttonArray = buttonArray;
    interface->numButtons = numButtons;
    
    struct up_ui_textArray *textArray = NULL;
    if (numText > 0) {
        textArray = malloc(sizeof(struct up_ui_textArray)*numText);
        if (textArray == NULL)
        {
            UP_ERROR_MSG("malloc failed");
            return NULL;
        }
    }
    interface->textArray = textArray;
    interface->numText = numText;
    
    struct up_ui_textFieldArray *textFieldArray = NULL;
    interface->active_textfield = noActiveTextField_menu;
    if (numTextFields > 0) {
        textFieldArray = malloc(sizeof(struct up_ui_textFieldArray)*numTextFields);
        if (textFieldArray == NULL)
        {
            UP_ERROR_MSG("malloc failed");
            return NULL;
        }
        interface->active_textfield = usernameActive_menu;
    }
    interface->textFieldArray = textFieldArray;
    interface->numTextField = numTextFields;
    
    struct up_ui_rectArray *rectdArray = NULL;
    if (numTextFields > 0) {
        rectdArray = malloc(sizeof(struct up_ui_rectArray)*numRect);
        if (rectdArray == NULL)
        {
            UP_ERROR_MSG("malloc failed");
            return NULL;
        }
    }
    interface->rectArray = rectdArray;
    interface->numRect = numRect;
    return interface;
}


static void up_menu_interface_freeStorage(struct up_menu_interface *interface)
{
    free(interface->buttonArray);
    free(interface->textArray);
    free(interface->textFieldArray);
    free(interface->rectArray);
    
    interface->buttonArray = NULL;
    interface->textArray = NULL;
    interface->textFieldArray = NULL;
    interface->rectArray = NULL;
    //this is importent, simplefies deallocation gaurds, and prevents programmer errors hopfully
    interface->numButtons = 0;
    interface->numText = 0;
    interface->numTextField = 0;
    interface->numRect = 0;
}

#define UP_BUTTON_KEYNAME_MAX 20

// key bindings
struct keybinding_state
{
    int change_key;
    int bind_key;
};

int up_buttonPressed_keybind(int idx,void *data)
{
    struct keybinding_state *state = (struct keybinding_state*)data;
    if (state != NULL) {
        state->bind_key = idx;  //what key that should be changed
        state->change_key = 1;  // that a key has been selected
    }
    return 0;
}

void up_bindKey(struct up_key_map *keymap,struct up_ui_buttonArray *buttonArray,struct keybinding_state *bindstate,SDL_Keycode key)
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


static struct up_menu_interface *up_menu_interface_keyBinding_new(struct up_menu_interface *interface,
                                                                  struct up_key_map *keymap,
                                                                  struct up_ui_clickArea clickArea,
                                                                  struct up_vec3 textScale,float step,
                                                                  struct keybinding_state *state_data,
                                                                  struct up_font_assets *fonts)
{
    int count = 0;
    int i = 0;
    struct up_vec3 pos = clickArea.pos;
    while(keymap[count].key != 0)
    {
        count++;
    }
    if (count <=0) {
        return NULL;
    }
    
    interface = up_menu_interface_newStorage(interface,count, count, 0,0);
    if (interface == NULL) {
        return NULL;
    }
    
    struct up_texture *textureButton = up_load_texture("button_keybind");
    
    // the controller binds a action and a data ptr to the button,
    struct up_ui_controller_glue *controller = malloc(sizeof(struct up_ui_controller_glue)*(count + 1));
    if (controller == NULL) {
        UP_ERROR_MSG("controller failed, malloc fualt");
        return interface;
    }
    
    char *str_tmp = NULL;
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_button *button_tmp = NULL;
    struct up_vec3 textPos = {0};
    
    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    struct up_vec3 black_color = {0.0, 0.0, 0.0};
    
    for (i = 0; i < count; i++) {
        
        textPos = pos;
        textPos.x += -0.5;
        str_tmp = keymap[i].name;
        interface->textArray[i].elm = up_ui_text_new(str_tmp, (int)strlen(str_tmp), 0, textPos, white_color, textScale, 0, fonts);
        // this is done, becouse the str length varies between keys, ex 'k' abd space_bar big diff in length
        str_tmp = up_str_newSize(UP_BUTTON_KEYNAME_MAX);
        strncpy(str_tmp, SDL_GetKeyName(keymap[i].key), UP_BUTTON_KEYNAME_MAX);
        str_tmp[UP_BUTTON_KEYNAME_MAX -1] = '\0';   //strncpy dont garanti null termination string
        text_tmp = up_ui_text_new(str_tmp, UP_BUTTON_KEYNAME_MAX, 0, pos, black_color, textScale, 0, fonts);
        
        button_tmp = up_ui_button_new(pos, clickArea.width, clickArea.hight, NULL, text_tmp);
        button_tmp->texture = textureButton; // this reuses the same texture for all buttons
        
        controller[i].clickArea = &button_tmp->area;    // the area that can be clicked on
        controller[i].func = up_buttonPressed_keybind;  // function that is called when this button is prest
        controller[i].data = state_data;    // when the button is pressed this data is passed to func, along with idx
        
        interface->buttonArray[i].elm = button_tmp;
        
        pos.y -= step;
    }
    //last element of the controller needs to be NULL
    controller[count].clickArea = NULL;
    controller[count].func = NULL;
    controller[count].data = NULL;
    interface->controller_glue = controller;
    return interface;
}

static void up_menu_interface_keyBinding_free(struct up_menu_interface *interface)
{
    struct up_ui_textArray *textArray = interface->textArray;
    int numText = interface->numText;
    int i = 0;
    for (i = 0; i < numText; i++) {
        up_ui_text_free(textArray[i].elm, 0);   // we do not want to free the textBuffer its in keymapp!!
    }
    
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_buttonArray *buttonArray = interface->buttonArray;
    int numButtons = interface->numButtons;
    
    for (i = 0; i < numButtons; i++) {
        text_tmp = up_ui_button_free(buttonArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 1);   // this time we want to free the textbuffer also
    }
    
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
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
        *state->activeInterface = keyBindMenu;
    }
    
    return 0;
}


static struct up_menu_interface *up_menu_interface_settings_new(struct up_menu_interface *interface,
                                                                struct up_menu_data *menu_data,
                                                                struct up_ui_clickArea clickArea,
                                                                struct up_vec3 textScale,
                                                                struct up_font_assets *fonts)
{
    int numButtons = 3;
    struct up_vec3 pos = clickArea.pos;
    // 3 buttons, 0 text, 0 textfields, 1 rect
    interface = up_menu_interface_newStorage(interface, numButtons, 0, 0, 1);
    if (interface == NULL) {
        return NULL;
    }
    struct up_vec3 rectPos = pos;
    rectPos.z +=0.01;   // need to be behind buttons
    int rectWidth = clickArea.width*1.2;
    int rectHight = clickArea.hight*3*1.2;
    interface->rectArray[0].elm = up_ui_rect_new(rectPos, rectWidth, rectHight, "menu_black_square");
    
    struct up_texture *textureButton = up_load_texture("menu_white_square");
    struct up_vec3 black_color = {0.0, 0.0, 0.0};
    // view setup
    struct up_ui_textArray textArray[3] = {0};
    textArray[0].elm = up_ui_text_new("Toggel music", 0, 0, pos, black_color, textScale, 0, fonts);
    textArray[1].elm = up_ui_text_new("Toggle sound effects", 0, 0, pos, black_color, textScale, 0, fonts);
    textArray[2].elm = up_ui_text_new("Keybindings", 0, 0, pos, black_color, textScale, 0, fonts);
    int i = 0;
    struct up_ui_buttonArray buttonArray[3] = {0};
    for (i = 0; i < 3; i++) {
        buttonArray[i].elm = up_ui_button_new(pos, clickArea.width, clickArea.hight, NULL, textArray[i].elm);
        buttonArray[i].elm->texture = textureButton;
        interface->buttonArray[i].elm = buttonArray[i].elm;
        pos.y -= 0.1;
    }
    // controller setup
    // the controller binds a action and a data ptr to the button,
    struct up_ui_controller_glue *controller = malloc(sizeof(struct up_ui_controller_glue)*(numButtons + 1));
    if (controller == NULL) {
        UP_ERROR_MSG("controller failed, malloc fualt");
        return interface;
    }
    controller[0] = up_ui_controller_setGlue(&buttonArray[0].elm->area, up_buttonPressed_toggleMusic, menu_data);
    controller[1] = up_ui_controller_setGlue(&buttonArray[1].elm->area, up_buttonPressed_toggleSound, menu_data);
    controller[2] = up_ui_controller_setGlue(&buttonArray[2].elm->area, up_buttonPressed_keySettings, menu_data);
    controller[numButtons] = up_ui_controller_setGlue(NULL, NULL, NULL);
    interface->controller_glue = controller;
    return interface;
}

static void up_menu_interface_settings_free(struct up_menu_interface *interface)
{
    int i = 0;
    
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_buttonArray *buttonArray = interface->buttonArray;
    int numButtons = interface->numButtons;
    
    for (i = 0; i < numButtons; i++) {
        text_tmp = up_ui_button_free(buttonArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 1);   // this time we want to free the textbuffer also
    }
    
    up_ui_rect_free(interface->rectArray[0].elm);
    
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
}

//////////// registartion, and so on

static int textfield_selected(int idx,void *data)
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

//up_ui_clickArea
static struct up_menu_interface *up_menu_interface_account_new(struct up_menu_interface *interface,
                                                               char *title,
                                                                struct up_menu_data *menu_data,
                                                                struct up_ui_clickArea clickArea,
                                                                struct up_vec3 textScale,
                                                                struct up_font_assets *fonts)
{
    int numText = 2;
    int numTextfields = 2;
    struct up_vec3 pos = clickArea.pos;
    // 0 buttons, 2 text, 2 textfields, 1 rect
    interface = up_menu_interface_newStorage(interface, 0, numText, numTextfields, 1);
    if (interface == NULL) {
        return NULL;
    }
    struct up_vec3 rectPos = pos;
    rectPos.z +=0.01;   // need to be behind textfields
    int rectWidth = clickArea.width*1.2;
    int rectHight = clickArea.hight*3*1.2;
    interface->rectArray[0].elm = up_ui_rect_new(rectPos, rectWidth, rectHight, "menu_black_square");
    
    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    struct up_vec3 black_color = {0.0, 0.0, 0.0};
    struct up_vec3 title_pos = pos;
    
    title_pos.y +=0.1;
    interface->textArray[0].elm = up_ui_text_new(title, 0, 0, pos, white_color, textScale, 0, fonts);
    interface->textArray[1].elm = up_ui_text_new("Enter username and password", 0, 0, pos, white_color, textScale, 0, fonts);
    pos.y -= 0.1;
    
    // this is directly linked to the username,
    struct up_ui_text *username = up_ui_text_new(menu_data->user_data->name, UP_NAME_MAX_LENGTH, 0, pos, black_color, textScale, 0, fonts);
    struct up_ui_textField *user_field = up_ui_textField_new(pos, clickArea.width, clickArea.hight, "menu_white_square", username);
    interface->textFieldArray[0].elm = user_field;
    pos.y -= 0.1;
    
    // this is directly linked to the username,
    struct up_ui_text *password = up_ui_text_new(menu_data->user_data->password, UP_NAME_MAX_LENGTH, 1, pos, black_color, textScale, 0, fonts);
    struct up_ui_textField *pass_field = up_ui_textField_new(pos, clickArea.width, clickArea.hight, "menu_white_square", password);
    interface->textFieldArray[1].elm = pass_field;
    
    // the controller binds a action and a data ptr to the button,
    struct up_ui_controller_glue *controller = malloc(sizeof(struct up_ui_controller_glue)*(numTextfields + 1));
    if (controller == NULL) {
        UP_ERROR_MSG("controller failed, malloc fualt");
        return interface;
    }
    controller[0] = up_ui_controller_setGlue(&user_field->area, textfield_selected, interface);
    controller[1] = up_ui_controller_setGlue(&pass_field->area, textfield_selected, interface);
    controller[numTextfields] = up_ui_controller_setGlue(NULL, NULL, NULL);
    interface->controller_glue = controller;
    return interface;
}

static void up_menu_interface_account_free(struct up_menu_interface *interface)
{
    struct up_ui_textArray *textArray = interface->textArray;
    int numText = interface->numText;
    int i = 0;
    for (i = 0; i < numText; i++) {
        up_ui_text_free(textArray[i].elm, 1);   // we do want to free the textBuffer
    }
    
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_textFieldArray *textFieldArray = interface->textFieldArray;
    int numTextFields= interface->numTextField;
    
    for (i = 0; i < numTextFields; i++) {
        text_tmp = up_ui_textField_free(textFieldArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 0);   // this time we do not want to free the textbuffer
    }
    
    up_ui_rect_free(interface->rectArray[0].elm);
    
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
}
//////////// main menu

int up_menu_mainNavigation(int idx,void *data)
{
    struct up_menu_data *menu_data = (struct up_menu_data *)data;
    if (menu_data != NULL) {
        switch (idx) {
            case 0: *menu_data->menuExitFlags = 2; break;
            case 1: *menu_data->activeInterface = loginMenu; break;
            case 2: *menu_data->activeInterface = registrateMenu; break;
            case 3: *menu_data->activeInterface = settingsMenu; break;
            default:
                break;
        }
    }
    return 0;
}

static struct up_menu_interface *up_menu_interface_main_new(struct up_menu_interface *interface,
                                                               struct up_menu_data *menu_data,
                                                               struct up_ui_clickArea clickArea,
                                                               struct up_vec3 textScale,
                                                               struct up_font_assets *fonts)
{
    int numButtons = 3;
    int numRect = 1;
    struct up_vec3 pos = clickArea.pos;
    interface = up_menu_interface_newStorage(interface, numButtons, 0, 0, numRect);
    if (interface == NULL) {
        return NULL;
    }
    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    struct up_texture *textureButton = up_load_texture("menu_white_square");
    
    struct up_ui_textArray textArray[3] = {0};
    textArray[0].elm = up_ui_text_new("run local", 0, 0, pos, white_color, textScale, 0, fonts);
    textArray[1].elm = up_ui_text_new("LOGIN", 0, 0, pos, white_color, textScale, 0, fonts);
    textArray[2].elm = up_ui_text_new("REGISTRATE", 0, 0, pos, white_color, textScale, 0, fonts);
    int i = 0;
    struct up_ui_buttonArray buttonArray[3] = {0};
    for (i = 0; i < 3; i++) {
        buttonArray[i].elm = up_ui_button_new(pos, clickArea.width, clickArea.hight, NULL, textArray[i].elm);
        buttonArray[i].elm->texture = textureButton;
        interface->buttonArray[i].elm = buttonArray[i].elm;
        pos.y -= 0.1;
    }
    
    struct up_vec3 settings_pos = {0.8,-0.5,pos.z};
    struct up_ui_rect *settings = up_ui_rect_new(settings_pos, 400, 400, "cogWheel");
    interface->rectArray[0].elm = settings;
    
    // the controller binds a action and a data ptr to the button,
    int controllLength = numButtons + numRect;
    struct up_ui_controller_glue *controller = malloc(sizeof(struct up_ui_controller_glue)*(controllLength + 1));
    if (controller == NULL) {
        UP_ERROR_MSG("controller failed, malloc fualt");
        return interface;
    }
    for (i = 0; i < 3; i++) {
        controller[i] = up_ui_controller_setGlue(&buttonArray[i].elm->area, up_menu_mainNavigation, menu_data);
    }
    controller[3] = up_ui_controller_setGlue(&settings->area, up_menu_mainNavigation, menu_data);
    controller[numButtons] = up_ui_controller_setGlue(NULL, NULL, NULL);
    interface->controller_glue = controller;
    return interface;
}

static void up_menu_interface_main_free(struct up_menu_interface *interface)
{
    int i = 0;
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_buttonArray *buttonArray = interface->buttonArray;
    int numButtons = interface->numButtons;
    
    for (i = 0; i < numButtons; i++) {
        text_tmp = up_ui_button_free(buttonArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 1);   // this time we want to free the textbuffer also
    }
    up_ui_rect_free(interface->rectArray[0].elm);
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
}



