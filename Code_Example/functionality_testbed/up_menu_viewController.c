//
//  up_menu_viewController.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-16.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_menu_viewController.h"

#include <stdio.h>
#include "up_ui_module.h"
#include "up_music.h"
#include "up_sdl_redirect.h"
#include "up_math.h"
#include "up_control_events.h"
#include "up_error.h"
#include "up_network_module.h"


//magnus
struct up_menu_interface *up_menu_interface_newStorage(struct up_menu_interface *interface,int numButtons,int numText,int numTextFields,int numRect)
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
    
    struct up_ui_rectArray *rectArray = NULL;
    if (numRect > 0) {
        rectArray = malloc(sizeof(struct up_ui_rectArray)*numRect);
        if (rectArray == NULL)
        {
            UP_ERROR_MSG("malloc failed");
            return NULL;
        }
    }
    interface->rectArray = rectArray;
    interface->numRect = numRect;
    return interface;
}


void up_menu_interface_freeStorage(struct up_menu_interface *interface)
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


struct up_menu_interface *up_menu_interface_keyBinding_new(struct up_menu_interface *interface,
                                                                  struct up_key_map *keymap,
                                                                  struct up_ui_clickArea clickArea,
                                                                  struct up_vec3 textScale,float step,
                                                                  struct up_keybinding_state *state_data,
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

void up_menu_interface_keyBinding_free(struct up_menu_interface *interface)
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


/// settings

struct up_menu_interface *up_menu_interface_settings_new(struct up_menu_interface *interface,
                                                                struct up_menu_data *menu_data,
                                                                struct up_ui_clickArea clickArea,
                                                                struct up_vec3 textScale,
                                                                struct up_font_assets *fonts)
{
    int numButtons = 3;
    struct up_vec3 pos = clickArea.pos;
    // 3 buttons, 0 text, 0 textfields, 1 rect
    interface = up_menu_interface_newStorage(interface, numButtons, 0, 0, 0);
    if (interface == NULL) {
        return NULL;
    }
    
    struct up_texture *textureButton = up_load_texture("menu_black_square");
    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    // view setup
    struct up_ui_textArray textArray[3] = {0};
    textArray[0].elm = up_ui_text_new("Toggel music", 0, 0, pos, white_color, textScale, 0, fonts);
    textArray[1].elm = up_ui_text_new("Toggle sound", 0, 0, pos, white_color, textScale, 0, fonts);
    textArray[2].elm = up_ui_text_new("Keybindings", 0, 0, pos, white_color, textScale, 0, fonts);
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

void up_menu_interface_settings_free(struct up_menu_interface *interface)
{
    int i = 0;
    
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_buttonArray *buttonArray = interface->buttonArray;
    int numButtons = interface->numButtons;
    
    for (i = 0; i < numButtons; i++) {
        text_tmp = up_ui_button_free(buttonArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 1);   // this time we want to free the textbuffer also
    }
    
    for (i = 0; i < interface->numRect; i++) {
        up_ui_rect_free(interface->rectArray[i].elm);
    }
    
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
}

//////////// registartion, and so on



//up_ui_clickArea
struct up_menu_interface *up_menu_interface_account_new(struct up_menu_interface *interface,
                                                               char *title,
                                                               struct up_menu_data *menu_data,
                                                               struct up_ui_clickArea clickArea,
                                                               struct up_vec3 textScale,
                                                               struct up_font_assets *fonts)
{
    int numText = 2;
    int numTextfields = 2;
    struct up_vec3 pos = clickArea.pos;
    // 0 buttons, 2 text, 2 textfields, 0 rect
    interface = up_menu_interface_newStorage(interface, 0, numText, numTextfields, 0);
    if (interface == NULL) {
        return NULL;
    }

    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    struct up_vec3 black_color = {0.0, 0.0, 0.0};
    struct up_vec3 title_pos = pos;
    
    title_pos.x -= ((float)clickArea.width/UP_SCREEN_WIDTH);
    title_pos.y +=0.1;
    interface->textArray[0].elm = up_ui_text_new(title, 0, 0, title_pos, white_color, textScale, 0, fonts);
    title_pos.y -= 0.1;
    interface->textArray[1].elm = up_ui_text_new("Enter username and password", 0, 0, title_pos, white_color, textScale, 0, fonts);
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

void up_menu_interface_account_free(struct up_menu_interface *interface)
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
    for (i = 0; i < interface->numRect; i++) {
        up_ui_rect_free(interface->rectArray[i].elm);
    }
    free(interface->controller_glue);
    up_menu_interface_freeStorage(interface);
}

//////////// main menu

struct up_menu_interface *up_menu_interface_main_new(struct up_menu_interface *interface,
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
    struct up_texture *textureButton = up_load_texture("menu_black_square");
    
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
    
    struct up_vec3 settings_pos = {0.8,-0.7,pos.z};
    struct up_ui_rect *settings = up_ui_rect_new(settings_pos, 200, 200, "cogWheel");
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
    controller[controllLength] = up_ui_controller_setGlue(NULL, NULL, NULL);
    interface->controller_glue = controller;
    return interface;
}

void up_menu_interface_main_free(struct up_menu_interface *interface)
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



struct up_menu_interface *up_menu_interface_connecting_new(struct up_menu_interface *interface,
                                                     struct up_menu_data *menu_data,
                                                     struct up_ui_clickArea clickArea,
                                                     struct up_vec3 textScale,
                                                     struct up_font_assets *fonts)
{
    int numButtons = 1;
    interface = up_menu_interface_newStorage(interface, numButtons, 0, 0, 0);
    if (interface == NULL) {
        return NULL;
    }
    struct up_vec3 white_color = {1.0, 1.0, 1.0};
    struct up_ui_textArray textArray[1] = {0};
    textArray[0].elm = up_ui_text_new("Connecting...", 0, 0, clickArea.pos, white_color, textScale, 0, fonts);
    interface->buttonArray[0].elm = up_ui_button_new(clickArea.pos, clickArea.width, clickArea.hight, "menu_black_square", textArray[0].elm);

    return interface;
}

void up_menu_interface_connecting_free(struct up_menu_interface *interface)
{
    int i = 0;
    struct up_ui_text *text_tmp = NULL;
    struct up_ui_buttonArray *buttonArray = interface->buttonArray;
    int numButtons = interface->numButtons;
    
    for (i = 0; i < numButtons; i++) {
        text_tmp = up_ui_button_free(buttonArray[i].elm);   // returns the text in button
        up_ui_text_free(text_tmp, 1);   // this time we want to free the textbuffer also
    }
    up_menu_interface_freeStorage(interface);
}


