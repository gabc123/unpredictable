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
    struct up_objectInfo heart = up_asset_createObjFromId(8);


//    healthBarGreen.scale.x = 1;
//    healthBarGreen.scale.y = 1;
//    healthBarGreen.scale.z = 1;
//
//    healthBarRed.scale.x = 1;
//    healthBarRed.scale.y = 1;
//    healthBarRed.scale.z = 1;

    green_and_red.greenIndex = up_unit_add(up_others_type,healthBarGreen);
    green_and_red.redIndex = up_unit_add(up_others_type,healthBarRed);
    green_and_red.heartIndex = up_unit_add(up_others_type,heart);




    return green_and_red;


}


void moveHealthBar(int ship_id,up_health_bar_t healing)
{
    int healthLevel = 70;
    
    struct up_objectInfo *ship_pos = up_unit_objAtIndex(up_ship_type,ship_id);

    struct up_objectInfo *healthGreen = up_unit_objAtIndex(up_others_type,healing.greenIndex);
    struct up_objectInfo *healthRed = up_unit_objAtIndex(up_others_type,healing.redIndex);
    struct up_objectInfo *heart = up_unit_objAtIndex(up_others_type,healing.heartIndex);


    healthGreen->pos.x = ship_pos->pos.x-1;
    healthGreen->pos.y = ship_pos->pos.y+7;
    healthGreen->pos.z = ship_pos->pos.z;

    healthRed->pos.x = ship_pos->pos.x-1;
    healthRed->pos.y = ship_pos->pos.y+7;
    healthRed->pos.z = ship_pos->pos.z;
    
    heart->pos.x = ship_pos->pos.x+33;
    heart->pos.y = ship_pos->pos.y-28;
    heart->pos.z = ship_pos->pos.z;
    

    healthGreen->scale.x = ((float)healthLevel/100)*2;
    
}

void up_gamePlayInterface(struct up_font_assets *font_assets,struct shader_module *shader_program,int current_health,int max_health,
                          int current_armor,int max_armor){
    
    struct up_vec3 pos;
    struct up_vec3 scale;
    
    char ship_health[10];
    char ship_armor[10];
    
    pos.x = -0.92;
    pos.y = -0.72;
    pos.z = 0.0;
    
    scale.x = 0.0215;
    scale.y = 0.0215;
    scale.z = 0.0215;
    

    sprintf(ship_health,"%d/%d", current_health,max_health);
    sprintf(ship_armor,"%d/%d", current_armor,max_armor);

    
    int length = (int)strlen(ship_health);
    
    struct up_vec3 color = {0.0,8.0,3.0};
    
    up_displayText(ship_health,length, &pos, &scale, font_assets, shader_program,0.0002,&color);
    
    pos.x = -0.72;
    pos.y = -0.72;
    pos.z = 0.0;
    
    up_displayText(ship_armor,length, &pos, &scale, font_assets, shader_program,0.0002,&color);

}



