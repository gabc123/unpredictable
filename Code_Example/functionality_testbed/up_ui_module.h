//
//  up_ui_module.h
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_UI_MODULE_H
#define UP_UI_MODULE_H

#include "up_assets.h"
#include "up_math.h"
#include "up_sdl_redirect.h"

// move somewhere else, this is the window resulution
#define UP_SCREEN_WIDTH 1280
#define UP_SCREEN_HIGHT 800

// text containing all info needed to be display
struct up_ui_text
{
    int fieldId;
    char *text;
    int length;
    int max_size;
    int isPassword; // if set to 1, will only dusplay *** insead of text string
    struct up_vec3 pos;
    struct up_vec3 color;
    struct up_vec3 scale;
    float stepSize; // the diff in step between letters from defualt
    struct up_font_assets *fonts;
};

// a area that can be clicked
struct up_ui_clickArea
{
    struct up_vec3 pos;
    int width;
    int hight;
};

// button that can be clicked
struct up_ui_button
{
    struct up_ui_clickArea area;    // this describe the area that can be clicked, and the suze fir nesg
    struct up_mesh *mesh;           // model to be renderd, (a square taking up the whole screen, that are resized after area)
    struct up_texture *texture;     // texture to be used
    struct up_ui_text *text;  // if the button should have any text, this us the text
};

struct up_ui_textField
{
    struct up_ui_clickArea area;    // this describe the area that can be clicked, and the suze fir nesg
    struct up_mesh *mesh;           // model to be renderd, (a square taking up the whole screen, that are resized after area)
    struct up_texture *texture;     // texture to be used
    struct up_ui_text *text;  // if the button should have any text, this us the text
};

// a rectangle that you can add a texture too
struct up_ui_rect
{
    struct up_ui_clickArea area;
    struct up_mesh *mesh;
    struct up_texture *texture;
};

// makes it easier to make a array of diffrent textfields, and remove the needs for ** ptrs
// ex: struct up_ui_textFieldArray array[10];
// array[5].elm = up_ui_textField_new(pos,width,hight,"lala",text);

struct up_ui_textArray
{
    struct up_ui_text *elm;
};

struct up_ui_buttonArray
{
    struct up_ui_button *elm;
};

struct up_ui_textFieldArray
{
    struct up_ui_textField *elm;
};

struct up_ui_rectArray
{
    struct up_ui_rect *elm;
};


//helper functions
// creates a new string from str on the heap, so it can be used with ui_text
// str must be null terminated
char *up_str_new(const char *str);
char *up_str_newSize(int size);
void up_str_copy(char *dst,char*src,unsigned int size); //safer then strncpy (size < maxsize(dst))

/// functions to manage the ui elements


// pass a ptr to a sting that should be displayed, its length
// if maxLengt == 0, then the text will be copyied into a new allocated memory!,
// if isPassword == 1 then all text will be displayed with '*' instead
// stepSize is the space between letters, this is added to the defualt step length
// pass in the font assets that should be used by this text.
struct up_ui_text *up_ui_text_new(char *text,int maxLength,
                                  int isPassword,
                                  struct up_vec3 pos,
                                  struct up_vec3 color,
                                  struct up_vec3 scale,
                                  float stepSize,
                                  struct up_font_assets *fonts);

// frees the memory allocated by new,
//set freeTextBuffer to 1 if you want to free the textBuffer also,
// warning: the text buffer was not allocated by new, but the text you provided a pointer to when calling new
void up_ui_text_free(struct up_ui_text *text,int freeTextBuffer);

////////////////////////////
// this creates a textField, a text fieild has a clickable Area and texture , also a textfield to modify
// use this to create text input areas that need to be clickable
// pass in a ui_text that should be assosieted with this one,
// input:   pos = location on screen, in opengl coordinates
//          width,hight = given in pixels not screen coords
//          text is a ui_text element, to handel the text, however text internal pos will be overriden
//          textureName, if given NULL, it will be the users responsibility to set it,
//              this option can be used to reuse a texture instead of reloading it to every textfield
struct up_ui_textField *up_ui_textField_new(struct up_vec3 pos,int width,int hight,char *textureName,struct up_ui_text *text);


// returns the text that was inside textField incase you need to handel its memory lifetime
struct up_ui_text *up_ui_textField_free(struct up_ui_textField *textField);


//////////////////////////////
// creates a button to use, pass in the texture name, used the same way ui_textField_new works
struct up_ui_button *up_ui_button_new(struct up_vec3 pos,int width,int hight,char *textureName,struct up_ui_text *text);

// returns the text that was inside button incase you need to handel it diffrently
struct up_ui_text *up_ui_button_free(struct up_ui_button *button);


//////////////////////////////
// creates a rect to use, pass in the texture name, used the same way ui_textField_new works
struct up_ui_rect *up_ui_rect_new(struct up_vec3 pos,int width,int hight,char *textureName);

// returns the text that was inside button incase you need to handel it diffrently
void up_ui_rect_free(struct up_ui_rect *rect);



////////////////////////////////////////////

// renders one text, or textfield or button, or rect, with shader
void up_ui_text_render(struct up_ui_text *text,struct up_shader_module *shaderprog);
void up_ui_textField_render(struct up_ui_textField *textField,struct up_shader_module *shaderprog);
void up_ui_button_render(struct up_ui_button *button,struct up_shader_module *shaderprog);
void up_ui_rect_render(struct up_ui_rect *rect,struct up_shader_module *shaderprog);

/// array renders, renders a whole array of elements

void up_ui_textArray_render(struct up_ui_textArray *textArray,int num,struct up_shader_module *shaderprog);
void up_ui_textFieldArray_render(struct up_ui_textFieldArray *textFieldArray,int num,struct up_shader_module *shaderprog);
void up_ui_buttonArray_render(struct up_ui_buttonArray *buttonArray,int num,struct up_shader_module *shaderprog);
void up_ui_rectArray_render(struct up_ui_rectArray *rectArray,int num,struct up_shader_module *shaderprog);

/////////////////////////////
// check a clickable area if some one has clicked on it,
// its better to use up_ui_controller_update instead
int up_ui_check_clickArea(struct up_ui_clickArea clickAreaa,int mouse_x, int mouse_y);



///////////// controller functionaity
/*
    Use this structure to bind a clickable area to a function and its data
    By createing an array of this ending with  a null element, you can call controller_update
    and it will check everything and call the rigth stuff, magic =)
 */
struct up_ui_controller_glue
{
    struct up_ui_clickArea *clickArea;  // the area the can be clicked and activate the function
    int (*func)(int idx,void *data);    // function pointer to the action if clickarea is clicked,idx is the index in the click array this was.
    void *data; // data that should be passed to the function
};

struct up_ui_controller_glue up_ui_controller_setGlue(struct up_ui_clickArea *clickArea,int (*func)(int idx,void *data),void *data);


// call this with the current controlArray containing all clickable areas in the screen
void up_ui_controller_updateClick(struct up_ui_controller_glue *controlArray,int mouse_x,int mouse_y);


///////////////////// same concept as above
// set what key shold be binded to what function, and a pointer to the data
struct up_ui_controller_keyInput
{
    SDL_Keycode key;
    int (*func)(void *data);
    void *data;
};

void up_ui_controller_updateKeyAction(struct up_ui_controller_keyInput *controllArray,SDL_Keycode key);


/*
 A example use of the controller , you create a array with all your clickable areas
 then what function they should bind to, then the data that should be passed to the functions
 
 
 // example all the arguments and data should be passed to the function with a struct
 struct demoData
 {
 int arg1;
 int arg2;
 };
 
 // this is some controll glue code, to bind the controller to the model
 // this is just an example, you dont need to do it this way,
 // but to keep a mvc designe this is probably a ok begining
 int demoGlue(int idx,void *data)
 {
 struct demoData *myData = (struct demoData *)data;
 int ret = 0;
 if (myData != NULL) {
 ret = someModelFunc(myData->arg1,myData->arg2);
 }
 return ret;
 }
 
 // create the data that should be bounded to the function
 struct demoData dummyDemoData1 = {...};
 // can come from whatever, a button, textfield and so one
 struct up_ui_clickArea dummyDemoClick2 = {...};
 ...
 
 // fill out the controller with all your clickable areas,functions and the data they should be connected too
 // the location in this array is equal to idx that the function recives
 struct up_ui_controller_glue glueArray[] = {
 {&dummyDemoClick,&demoGlue,&dummyDemoData},
 {&dummyDemoClick,&demoGlue,&dummyDemoData},
 ...
 ...
 ...
 {NULL,NULL,NULL}    // always end the controller array with a null element
 };
 
 then you use it by calling the controller update
 up_ui_controller_updateClick(glueArray,mouse_x,mouse_y);
 
 */



#endif /* defined(__unpredictable_xcode__up_ui_module__) */
