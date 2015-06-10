//
//  up_ui_module.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_ui_module.h"
#include "up_graphics_update.h"
#include "up_render_engine.h"
#include "up_error.h"
#include "testmodels.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// move somewhere else, this is the window resulution
#define UP_SCREEN_WIDTH 1280
#define UP_SCREEN_HIGHT 800

struct up_ui_text *up_ui_text_new(char *text,int maxLength,
                                  int isPassword,
                                  struct up_vec3 color,
                                  struct up_vec3 scale,
                                  float stepSize,
                                  struct up_font_assets *fonts)
{
    if (text == NULL) {
        UP_ERROR_MSG("passed null to ui_text_new");
        maxLength = 0;
    }
    
    struct up_ui_text *uiText = malloc(sizeof(struct up_ui_text));
    if (uiText == NULL) {
        UP_ERROR_MSG("malloc failed");
        return  NULL;
    }
    int length = (int)strlen(text);
    length = (maxLength > length) ? length : maxLength;
    
    uiText->length = length;
    uiText->max_size = maxLength;
    uiText->text = text;
    
    uiText->color = color;
    uiText->scale = scale;
    uiText->stepSize = stepSize;
    uiText->isPassword = isPassword;
    uiText->fonts = fonts;
    uiText->fieldId = 0;
    
    return uiText;
}

struct up_ui_textField *up_ui_textField_new(struct up_vec3 pos,int width,int hight,char *textureName,struct up_ui_text *text)
{
    struct up_ui_textField *textField = malloc(sizeof(struct up_ui_textField));
    if (textField == NULL) {
        UP_ERROR_MSG("malloc failure");
        return NULL;
    }
    textField->area.pos = pos;
    textField->area.width = width;
    textField->area.hight = hight;
    
    struct up_mesh *mesh = up_mesh_menu_Botton();
    
    struct up_texture *texture = up_load_texture(textureName);
    if(texture == NULL){
        texture = up_load_texture("lala.png");
    }
    
    textField->mesh = mesh;
    textField->texture = texture;
    textField->text = text;
    return textField;
}

struct up_ui_button *up_ui_button_new(struct up_vec3 pos,int width,int hight,char *textureName,struct up_ui_text *text)
{
    struct up_ui_button *button = malloc(sizeof(struct up_ui_button));
    if (button == NULL) {
        UP_ERROR_MSG("malloc failure");
        return NULL;
    }
    button->area.pos = pos;
    button->area.width = width;
    button->area.hight = hight;
    
    struct up_mesh *mesh = up_mesh_menu_Botton();
    
    struct up_texture *texture = up_load_texture(textureName);
    if(texture == NULL){
        texture = up_load_texture("lala.png");
    }
    
    button->mesh = mesh;
    button->texture = texture;
    button->text = text;
    return button;
}


void up_ui_text_free(struct up_ui_text *text,int freeTextBuffer)
{
    if (text == NULL) {
        return; //dont need to free
    }
    // free the text buffer also
    if (freeTextBuffer == 1) {
        free(text->text);
    }
    
    text->text = NULL;
    free(text);
}

// returns the text that was inside button incase you need to handel it diffrently
struct up_ui_text *up_ui_button_free(struct up_ui_button *button)
{
    struct up_ui_text *tmp_text = button->text;
    button->text = NULL;
    free(button);
    return tmp_text;
}

// returns the text that was inside button incase you need to handel it diffrently
struct up_ui_text *up_ui_textField_free(struct up_ui_textField *textField)
{
    struct up_ui_text *tmp_text = textField->text;
    //the text is allocated somewhere else
    textField->text = NULL;
    // make it unclickavke just incase
    textField->area.hight = 0;
    textField->area.width = 0;
    textField->area.pos.x = -151.0;
    free(textField);
    
    return tmp_text;
}

////////////////////////////////////////////

void up_ui_text_render(struct up_vec3 pos,struct up_ui_text *text,struct up_shader_module *shaderprog)
{
    if (text == NULL) {
        return; // sometimes we dont need text, so the programmer dont need the think about it
    }
    char *hiddenText = "**********************************************";
    
    int length = text->length;
    char *tmp_text = text->text;
    // we want to hide the text if its a password or something mysterious
    if (text->isPassword == 1) {
        length = (int)strlen(hiddenText);
        tmp_text = hiddenText;
        // we only want to show the number of letters that has been typed
        // and aalso to prevent overflow
        length = (length > text->length) ? text->length : length;
    }
    
    // this renders the text to the display, yay ...
    up_displayText(tmp_text, length, &pos, &text->scale, text->fonts, shaderprog, text->stepSize, &text->color);
}

void up_ui_textField_render(struct up_ui_textField *textField,struct up_shader_module *shaderprog)
{
    if (textField == NULL) {
        return;
    }
    
    struct up_vec3 rot = {0};
    
    // becouse the area is given in pixels and not screen size, we need to convert it to screen pos
        struct up_vec3 scale = {0};
    scale.x = (float)textField->area.width/UP_SCREEN_WIDTH;
    scale.y = (float)textField->area.hight/UP_SCREEN_HIGHT;
    scale.z = 1;
    
    up_matrix4_t transform = {0};  //used by the shader to transforme the mesh to the correct loc on scrren
    
    up_matrixModel(&transform, &textField->area.pos, &rot, &scale);
    
    up_shader_update(shaderprog, &transform);
    up_texture_bind(textField->texture, 0);
    up_mesh_draw(textField->mesh);
    
    struct up_vec3 textPos = textField->area.pos;
    textPos.z -= 0.01;  //text need to be infront of the textfield model
    up_ui_text_render(textPos, textField->text, shaderprog);
}

void up_ui_button_render(struct up_ui_button *button,struct up_shader_module *shaderprog)
{
    if (button == NULL) {
        return;
    }
    
    struct up_vec3 rot = {0};
    
    // becouse the area is given in pixels and not screen size, we need to convert it to screen pos
    struct up_vec3 scale = {0};
    scale.x = (float)button->area.width/UP_SCREEN_WIDTH;
    scale.y = (float)button->area.hight/UP_SCREEN_HIGHT;
    scale.z = 1;
    
    up_matrix4_t transform = {0};  //used by the shader to transforme the mesh to the correct loc on scrren
    
    up_matrixModel(&transform, &button->area.pos, &rot, &scale);
    
    up_shader_update(shaderprog, &transform);
    up_texture_bind(button->texture, 0);
    up_mesh_draw(button->mesh);
    
    struct up_vec3 textPos = button->area.pos;
    textPos.z -= 0.01;  //text need to be infront of the textfield model
    up_ui_text_render(textPos, button->text, shaderprog);
}


/////////////////////////////
int up_ui_check_clickArea(struct up_ui_clickArea clickAreaa,int mouse_x, int mouse_y)
{
    int x = UP_SCREEN_WIDTH*(clickAreaa.pos.x + 1.0)/2;
    int y = UP_SCREEN_HIGHT - UP_SCREEN_HIGHT*(clickAreaa.pos.y + 1.0)/2;
    int width = clickAreaa.width;
    int hight = clickAreaa.hight;
    x = x - width/2; // the xy coord is the center of the button
    y = y - hight/2; // the xy coord is the center of the button
    
    // check if the click is inside the buttom, returns "true" elese "false"
    return ((x <= mouse_x) && ( mouse_x <= x + width) && (y <= mouse_y) && ( mouse_y <= y + hight));
}


////////////////////////////

void up_ui_controller_update(struct up_ui_controller_glue *controlArray,int mouse_x,int mouse_y)
{
    if (controlArray == NULL) {
        return;
    }
    int i = 0;
    void *data = controlArray[0].data;
    for (i = 0; controlArray[i].clickArea != NULL; i++) {
        if (up_ui_check_clickArea(*controlArray[i].clickArea, mouse_x, mouse_y)) {
            data = controlArray[i].data;
            controlArray[i].func(data); // currently we are not using the return value
        }
    }
    
}

