#include "up_healthbar.h"
#include "up_matrixTransforms.h"
#include "up_ship.h"
#include "up_menu.h"
#include <stdio.h>
#include "up_initGraphics.h"
#include "up_utilities.h"
#include "up_ship.h"
#include "up_objectReader.h"
#include "up_shader_module.h"
#include "up_network_module.h"
#include "testmodels.h"
#include "up_modelRepresentation.h"
#include "up_matrixTransforms.h"
#include "up_assets.h"



up_health_bar_t healthbar_creation()
{

    up_health_bar_t green_and_red;

    struct up_objectInfo healthBarGreen = up_asset_createObjFromId(6);
    struct up_objectInfo healthBarRed = up_asset_createObjFromId(5);

//    healthBarGreen.scale.x = 1;
//    healthBarGreen.scale.y = 1;
//    healthBarGreen.scale.z = 1;
//
//    healthBarRed.scale.x = 1;
//    healthBarRed.scale.y = 1;
//    healthBarRed.scale.z = 1;

    green_and_red.greenIndex = up_unit_add(up_others_type,healthBarGreen);
    green_and_red.redIndex = up_unit_add(up_others_type,healthBarRed);




    return green_and_red;


}


void moveHealthBar(int ship_id,up_health_bar_t healing)
{
    int healthLevel = 70;
    
    struct up_objectInfo *ship_pos = up_unit_objAtIndex(up_ship_type,ship_id);

    struct up_objectInfo *healthGreen = up_unit_objAtIndex(up_others_type,healing.greenIndex);
    struct up_objectInfo *healthRed = up_unit_objAtIndex(up_others_type,healing.redIndex);

    healthGreen->pos.x = ship_pos->pos.x-1;
    healthGreen->pos.y = ship_pos->pos.y+7;
    healthGreen->pos.z = ship_pos->pos.z;

    healthRed->pos.x = ship_pos->pos.x-1;
    healthRed->pos.y = ship_pos->pos.y+7;
    healthRed->pos.z = ship_pos->pos.z;
    

    healthGreen->scale.x = ((float)healthLevel/100)*2;
    
}

void up_gamePlayInterface(struct up_font_assets *font_assets,struct shader_module *shader_program){
    
    struct up_vec3 pos;
    struct up_vec3 scale;
    
    pos.x = -0.17;
    pos.y = -0.45;
    pos.z = -0.5;
    
    scale.x = 0.5;
    scale.y = 0.5;
    scale.z = 0.5;
    
    char *text = "hejkds<djcpdo<cszodps";
    int length = (int)strlen(text);
    
    up_displayText(text,length, &pos, &scale, font_assets, shader_program);

}



