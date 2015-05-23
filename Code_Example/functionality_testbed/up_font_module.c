//
//  up_font_module.c
//  testprojectshader
//
//  Created by o_0 on 2015-05-06.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_assets.h"
#include "up_error.h"
#include "up_texture_module.h"
#include "up_shader_module.h"
#include "up_matrixTransforms.h"
#include "up_vertex.h"
#include <stdlib.h>
#include <stdio.h>

void up_displayText(char *text_string,int length,struct up_vec3 *pos,
                    struct up_vec3 *scale,struct up_font_assets *fonts,
                    struct shader_module *shaderprog, float step,struct up_vec3 *color)
{
    struct up_vec3 localpos = *pos;
    up_matrix4_t transform;
    struct up_vec3 rot = {0};
    int index = 0;
    if (length < 1) {
        return;
    }
    UP_shader_bind(shaderprog);
    up_texture_bind(fonts->texture, 1);
    if (color != NULL) {
        up_shader_update_font_color(shaderprog,color);
    }

    
    int i = 0;
    for (i = 0; i< length; i++) {
         // flyttar os 1 plats
        up_matrixModel(&transform,&localpos,&rot,scale);
        UP_shader_update(shaderprog,&transform);
        index = text_string[i] - 22;
        up_draw_mesh(&fonts->letters[index]);
        //up_draw_mesh(fonts->); // skriver ut c
        localpos.x += 0.0225 + step;
        
        
    }
    struct up_vec3 clearColor = {0};
    if (color != NULL) {
        up_shader_update_font_color(shaderprog,&clearColor);
    }
    
}

//#define UP_LETTER_OFFSET 0.03811803751804

// image is 80 by 160, and have 10 by 10 chareckter
#define UP_LETTER_OFFSET_X 0.1
#define UP_LETTER_OFFSET_Y 0.1

//OLD 0.03846153846154

// width/hight
#define UP_LETTER_ASPECT_RATIO 0.535
static struct up_mesh *up_meshLetters(int letterIndex)
{
    /// setup the vertexs and the tex coords, this is done like this for debbuging reasons
    // texture coordinates, 0,0 is bottom left, 1,1 is top right
    
    if ((letterIndex < 0) || (letterIndex > UP_NUMBER_LETTER - 1))
    {
        printf("\noffset value %d",letterIndex);
        UP_ERROR_MSG("font posistion out of bounds");
        letterIndex = 0;
    }
    // the letter images is a 10 by 10 grid of letters and charecters
    // this gets the correct x y coords, but the are not scaled yet
    float x_offset = (float)(letterIndex%10);
    float y_offset = (float)(letterIndex - (letterIndex%10))/10.0f;
    
    x_offset = x_offset * UP_LETTER_OFFSET_X; // scale to the rigth tex coords
    // the y coord start at top
    y_offset = 1.0 - y_offset * UP_LETTER_OFFSET_Y; // scale to the rigth tex coords
    
    struct up_vec2 tex[] = {
        {x_offset, y_offset},
        {UP_LETTER_OFFSET_X + x_offset, y_offset},
        {x_offset, UP_LETTER_OFFSET_Y + y_offset},
        {UP_LETTER_OFFSET_X + x_offset, UP_LETTER_OFFSET_Y + y_offset}
    };
    
    // this is the posisions of the vertexes
    struct up_vec3 pos[] = {
        {-1.0f * UP_LETTER_ASPECT_RATIO, -1.0f, 0.01f},   // bottomleft 0
        {1.0f * UP_LETTER_ASPECT_RATIO, -1.0f, 0.01f},   //bottom right 1
        {-1.0f * UP_LETTER_ASPECT_RATIO, 1.0f, 0.01f},  //topleft 2
        {1.0f * UP_LETTER_ASPECT_RATIO, 1.0f, 0.01f}   //topright 3
    };
    
    
    unsigned int indexArray[] = {1,2,0,1,3,2};  //binds togheter two triangels into one square
    
    // left over from debugging. fills the vertex array with pos and tex
    struct up_vertex vertex[4];
    int i = 0;
    for (i = 0; i < 4; i++) {
        vertex[i].pos = pos[i];
        vertex[i].texCoord = tex[i];
        vertex[i].normals.x = 0;
        vertex[i].normals.y = 0;
        vertex[i].normals.z = 0;
    }
    /////////////
    printf("vertex start: ");
    struct up_mesh *mesh = UP_mesh_new(vertex, sizeof(vertex)/sizeof(vertex[0]),indexArray, sizeof(indexArray)/sizeof(indexArray[0]));
    //printf("Mesh finnished\n");
    return mesh;
}






struct up_font_assets * up_font_start_setup()
{
    struct up_font_assets *fonts = malloc(sizeof(struct up_font_assets));
    if (fonts == NULL) {
        UP_ERROR_MSG("font malloc failed");
        return NULL;
    }
    //Alphabet.png
    //struct up_texture_data *textureLetters = up_load_texture("letters.png");
    //struct up_texture_data *textureLetters = up_load_texture("letters_numbers.png");
    struct up_texture_data *textureLetters = up_load_texture("font_good_new");
    
    if (textureLetters==NULL) {
        textureLetters = up_load_texture("lala.png");
    }
    
    fonts->texture = textureLetters;
    

    
    struct up_mesh *tmp_mesh = NULL;
    
    int i = 0;
    for (i = 0; i < UP_NUMBER_LETTER; i++) {
        tmp_mesh = up_meshLetters(i);
        fonts->letters[i] = *tmp_mesh;
        
    }
    fonts->size = UP_NUMBER_LETTER;
    
    return fonts;
}



