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





char *up_str_new(const char *str)
{
    if (str == NULL) {
        return NULL;
    }
    int len = (int)strlen(str) + 1;
    
    char *text = malloc(sizeof(char) * len);
    if (text == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    //strncpy(text, str, len);
    text[0] = '\0';
    strncat(text, str, len -1);//always null terminate, instead of strncpy that is not requierd to do it
    return text;
}

char *up_str_newSize(int size)
{
    char *text = malloc(sizeof(char) * size);
    if (text == NULL) {
        UP_ERROR_MSG("malloc failed");
        return NULL;
    }
    return text;
}

// safer then strncpy,
void up_str_copy(char *dst,char*src,unsigned int size)
{
    dst[0] = '\0';
    strncat(dst, src, size);
}

struct up_ui_text *up_ui_text_new(char *text,int maxLength,
                                  int isPassword,
                                  struct up_vec3 pos,
                                  struct up_vec3 color,
                                  struct up_vec3 scale,
                                  float stepSize,
                                  struct up_font_assets *fonts)
{
    if (text == NULL) {
        UP_ERROR_MSG("passed null to ui_text_new");
        maxLength = 0;
    }
    
    if (maxLength == 0) {
        text = up_str_new(text);
        maxLength = (int)strlen(text);
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
    
    uiText->pos = pos;
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
    
    // incase you want to reuse the same texture, you dont want to reload it every time
    textField->texture = NULL;
    struct up_texture *texture = NULL;
    if (textureName != NULL) {
        texture = up_load_texture(textureName);
        if(texture == NULL){
            texture = up_load_texture("lala.png");
        }
    }
   
    
    textField->mesh = mesh;
    textField->texture = texture;
    
    
    struct up_vec3 textPos = textField->area.pos;
    textPos.x -= ((float)width/UP_SCREEN_WIDTH)/1.2;
    textPos.z -= 0.01;  //text need to be infront of the textfield model
    text->pos = textPos;
    
    
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
    
    // incase you want to reuse the same texture, you dont want to reload it every time
    button->texture = NULL;
    struct up_texture *texture = NULL;
    if (textureName != NULL) {
        texture = up_load_texture(textureName);
        if(texture == NULL){
            texture = up_load_texture("lala.png");
        }
    }
    
    button->mesh = mesh;
    button->texture = texture;

    
    struct up_vec3 textPos = button->area.pos;
    
    textPos.x -= ((float)width/UP_SCREEN_WIDTH)/1.2;
    //textPos.y = ((float)hight/UP_SCREEN_HIGHT)/2;
    textPos.z -= 0.01;  //text need to be infront of the textfield model
    text->pos = textPos;
    
    button->text = text;
    return button;
}

//////////////////////////////
// creates a rect to use, pass in the texture name, used the same way ui_textField_new works
struct up_ui_rect *up_ui_rect_new(struct up_vec3 pos,int width,int hight,char *textureName)
{
    struct up_ui_rect *rect = malloc(sizeof(struct up_ui_button));
    if (rect == NULL) {
        UP_ERROR_MSG("malloc failure");
        return NULL;
    }
    rect->area.pos = pos;
    rect->area.width = width;
    rect->area.hight = hight;
    
    struct up_mesh *mesh = up_mesh_menu_Botton();
    
    // incase you want to reuse the same texture, you dont want to reload it every time
    rect->texture = NULL;
    struct up_texture *texture = NULL;
    if (textureName != NULL) {
        texture = up_load_texture(textureName);
        if(texture == NULL){
            texture = up_load_texture("lala.png");
        }
    }
    
    rect->mesh = mesh;
    rect->texture = texture;

    return rect;
}




//////

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


// returns the text that was inside button incase you need to handel it diffrently
void up_ui_rect_free(struct up_ui_rect *rect)
{
    // prevent it to be renderd on screen, just incase
    rect->area.width = 0;
    rect->area.hight = 0;
    rect->area.pos.x = -123;
    free(rect);
}


////////////////////////////////////////////

void up_ui_text_render(struct up_ui_text *text,struct up_shader_module *shaderprog)
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
    up_displayText(tmp_text, length, &text->pos, &text->scale, text->fonts, shaderprog, text->stepSize, &text->color);
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
    
    up_ui_text_render(textField->text, shaderprog);
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
    
    up_ui_text_render(button->text, shaderprog);
}

void up_ui_rect_render(struct up_ui_rect *rect,struct up_shader_module *shaderprog)
{
    if (rect == NULL) {
        return;
    }
    
    struct up_vec3 rot = {0};
    
    // becouse the area is given in pixels and not screen size, we need to convert it to screen pos
    struct up_vec3 scale = {0};
    scale.x = (float)rect->area.width/UP_SCREEN_WIDTH;
    scale.y = (float)rect->area.hight/UP_SCREEN_HIGHT;
    scale.z = 1;
    
    up_matrix4_t transform = {0};  //used by the shader to transforme the mesh to the correct loc on scrren
    
    up_matrixModel(&transform, &rect->area.pos, &rot, &scale);
    
    up_shader_update(shaderprog, &transform);
    up_texture_bind(rect->texture, 0);
    up_mesh_draw(rect->mesh);
}
/// array renders

void up_ui_textArray_render(struct up_ui_textArray *textArray,int num,struct up_shader_module *shaderprog)
{
    if (textArray == NULL) {
        return; // sometimes we dont need text, so the programmer dont need the think about it
    }
    int i = 0;
    for (i = 0; i < num; i++) {
        up_ui_text_render(textArray[i].elm, shaderprog);
    }
}

void up_ui_textFieldArray_render(struct up_ui_textFieldArray *textFieldArray,int num,struct up_shader_module *shaderprog)
{
    if (textFieldArray == NULL) {
        return; // sometimes we dont need text, so the programmer dont need the think about it
    }
    int i = 0;
    for (i = 0; i < num; i++) {
        up_ui_textField_render(textFieldArray[i].elm, shaderprog);
    }
}

void up_ui_buttonArray_render(struct up_ui_buttonArray *buttonArray,int num,struct up_shader_module *shaderprog)
{
    if (buttonArray == NULL) {
        return; // sometimes we dont need text, so the programmer dont need the think about it
    }
    int i = 0;
    for (i = 0; i < num; i++) {
        up_ui_button_render(buttonArray[i].elm, shaderprog);
    }
}

void up_ui_rectArray_render(struct up_ui_rectArray *rectArray,int num,struct up_shader_module *shaderprog)
{
    if (rectArray == NULL) {
        return; // sometimes we dont need text, so the programmer dont need the think about it
    }
    int i = 0;
    for (i = 0; i < num; i++) {
        up_ui_rect_render(rectArray[i].elm, shaderprog);
    }
}



////////////////////////
struct up_ui_controller_glue up_ui_controller_setGlue(struct up_ui_clickArea *clickArea,int (*func)(int idx,void *data),void *data)
{
    struct up_ui_controller_glue controller = {0};
    controller.clickArea = clickArea;
    controller.func = func;
    controller.data = data;
    return controller;
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

void up_ui_controller_updateClick(struct up_ui_controller_glue *controlArray,int mouse_x,int mouse_y)
{
    if (controlArray == NULL) {
        return;
    }
    int i = 0;
    void *data = controlArray[0].data;
    for (i = 0; controlArray[i].clickArea != NULL; i++) {
        if (up_ui_check_clickArea(*controlArray[i].clickArea, mouse_x, mouse_y)) {
            data = controlArray[i].data;
            controlArray[i].func(i,data); // currently we are not using the return value
        }
    }
    
}


void up_ui_controller_updateKeyAction(struct up_ui_controller_keyInput *controllArray,SDL_Keycode key)
{
    if (controllArray == NULL || key == 0) {
        return;
    }
    
    int i = 0;
    void *data = controllArray[i].data;
    for (i =0; controllArray[i].key != 0; i++) {
        if (controllArray[i].key == key) {
            data = controllArray[i].data;
            controllArray[i].func(data);
        }
    }
    
}


