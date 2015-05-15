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

    
    healthBarGreen.angle = 0;
    healthBarRed.angle = 0;

    green_and_red.greenIndex = up_unit_add(up_others_type,healthBarGreen);
    green_and_red.redIndex = up_unit_add(up_others_type,healthBarRed);

    return green_and_red;


}


void up_moveHealthBar(int ship_id,up_health_bar_t green_and_red)
{
    int healthLevel = 70;
    
    struct up_objectInfo *ship_pos = up_unit_objAtIndex(up_ship_type,ship_id);

    struct up_objectInfo *healthGreen = up_unit_objAtIndex(up_others_type,green_and_red.greenIndex);
    struct up_objectInfo *healthRed = up_unit_objAtIndex(up_others_type,green_and_red.redIndex);

    healthGreen->pos.x = ship_pos->pos.x-1;
    healthGreen->pos.y = ship_pos->pos.y+7;
    healthGreen->pos.z = ship_pos->pos.z;

    healthRed->pos.x = ship_pos->pos.x-1;
    healthRed->pos.y = ship_pos->pos.y+7;
    healthRed->pos.z = ship_pos->pos.z;
    

    healthGreen->scale.x = ((float)healthLevel/100)*2;
    
}

up_stats_index_t up_create_statsObject()
{
    up_stats_index_t interfaceObject;
    
    struct up_objectInfo heart = up_asset_createObjFromId(8);
    struct up_objectInfo armor = up_asset_createObjFromId(10);
    struct up_objectInfo frameLeft = up_asset_createObjFromId(11);
    struct up_objectInfo frameRight = up_asset_createObjFromId(12);
    struct up_objectInfo bullet = up_asset_createObjFromId(13);
    struct up_objectInfo missile = up_asset_createObjFromId(14);
    struct up_objectInfo laser = up_asset_createObjFromId(15);

    
    heart.angle = 0;
    armor.angle = 0;
    frameLeft.angle = 0;
    frameRight.angle = 0;
    bullet.angle = 0;
    missile.angle = 0;
    laser.angle = 0;
    
    interfaceObject.heartIndex = up_unit_add(up_others_type,heart);
    interfaceObject.armorIndex = up_unit_add(up_others_type,armor);
    interfaceObject.frameLeftIndex = up_unit_add(up_others_type, frameLeft);
    interfaceObject.frameRightIndex = up_unit_add(up_others_type, frameRight);
    interfaceObject.bulletIndex = up_unit_add(up_others_type, bullet);
    interfaceObject.missileIndex = up_unit_add(up_others_type, missile);
    interfaceObject.laserIndex = up_unit_add(up_others_type, laser);



    return interfaceObject;
}

void up_interface_placement(struct up_camera *cam,up_stats_index_t interfaceObject)
{
    
    struct up_objectInfo *heart = up_unit_objAtIndex(up_others_type,interfaceObject.heartIndex);
    struct up_objectInfo *armor = up_unit_objAtIndex(up_others_type,interfaceObject.armorIndex);
//    struct up_objectInfo *frameLeft = up_unit_objAtIndex(up_others_type,interfaceObject.frameLeftIndex);
//    struct up_objectInfo *frameRight = up_unit_objAtIndex(up_others_type,interfaceObject.frameRightIndex);
    struct up_objectInfo *bullet = up_unit_objAtIndex(up_others_type,interfaceObject.bulletIndex);
    struct up_objectInfo *missile = up_unit_objAtIndex(up_others_type,interfaceObject.missileIndex);
    struct up_objectInfo *laser = up_unit_objAtIndex(up_others_type,interfaceObject.laserIndex);


    heart->pos = cam->eye;
    armor->pos = cam->eye;
//    frameLeft->pos = cam->eye;
//    frameRight->pos = cam->eye;
    bullet->pos = cam->eye;
    missile->pos = cam->eye;
    laser->pos = cam->eye;
    
    heart->pos.x += 3;             //interface plus
    heart->pos.y -= 2.3;
    heart->pos.z += 6;

    armor->pos.x += 3.01;          //inteface shield
    armor->pos.y -= 2.55;
    armor->pos.z += 6;
    
//    frameLeft->pos.x += 2.85;       //interface frames
//    frameLeft->pos.y -= 2.4;
//    frameLeft->pos.z +=6;
//    
//    frameRight->pos.x -= 2.58;
//    frameRight->pos.y -= 2.1;
//    frameRight->pos.z +=6;
    
    bullet->pos.x -= 2.77;             //interface bullet
    bullet->pos.y -= 2.05;
    bullet->pos.z +=6;
    
    missile->pos.x -= 2.77;             //interface missile
    missile->pos.y -= 2.3;
    missile->pos.z +=6;
    
    laser->pos.x -= 2.77;             //interface laser
    laser->pos.y -= 2.55;
    laser->pos.z +=6;
}



void up_gamePlayInterface(struct up_font_assets *font_assets,struct shader_module *shader_program,up_player_stats_t *stats){
    
    struct up_vec3 pos;
    struct up_vec3 scale;
    struct up_vec3 color;
    
    char ship_health[5];
    char ship_armor[5];
    char ship_bullet[5];
    char ship_missile[5];
    char ship_laser[5];


    
    pos.x = -0.85;
    pos.y = -0.7;
    pos.z = 0.0;
    
    scale.x = 0.04;
    scale.y = 0.04;
    scale.z = 0.04;
    

    sprintf(ship_health,"%d", stats->current_health);
    sprintf(ship_armor,"%d", stats->current_armor);
    sprintf(ship_bullet,"%d", stats->weapons.bullets);
    sprintf(ship_missile,"%d", stats->weapons.missile);
    sprintf(ship_laser,"%d", stats->weapons.laser);


    
    int length = (int)strlen(ship_health);                                      //Health
    
    if(stats->current_health <= 50){
        color.x = 20.118;
        color.y = 0;
        color.z = 0;
    }
    else{
        color.x = 20.118;
        color.y = 1;
        color.z = 1;
    }
    
    up_displayText(ship_health,length, &pos, &scale, font_assets, shader_program,0.002,&color);
    
    pos.x = -0.85;
    pos.y = -0.77;
    pos.z = 0.0;
    
    length = (int)strlen(ship_armor);                                           //armor
    
    if(stats->current_armor <= 50){
        color.x = 20.118;
        color.y = 0;
        color.z = 0;
    }
    else{
        color.x = 20.118;
        color.y = 1;
        color.z = 1;
    }
    
    up_displayText(ship_armor,length, &pos, &scale, font_assets, shader_program,0.002,&color);
    
    pos.x = 0.87;
    pos.y = -0.607;
    
    length = (int)strlen(ship_bullet);                                          //bullets
    
    if(stats->weapons.bullets <= 50){
        color.x = 20.118;
        color.y = 0;
        color.z = 0;
    }
    else{
        color.x = 20.118;
        color.y = 1;
        color.z = 1;
    }
    
    up_displayText(ship_bullet,length, &pos, &scale, font_assets, shader_program,0.002,&color);
    
    pos.x = 0.87;
    pos.y = -0.68;
    
    length = (int)strlen(ship_bullet);                                          //missile
    
    if(stats->weapons.missile <= 30){
        color.x = 20.118;
        color.y = 0;
        color.z = 0;
    }
    else{
        color.x = 20.118;
        color.y = 1;
        color.z = 1;
    }
    
    up_displayText(ship_missile,length, &pos, &scale, font_assets, shader_program,0.002,&color);

    pos.x = 0.87;
    pos.y = -0.758;
    
    length = (int)strlen(ship_bullet);                                          //laser
    
    if(stats->weapons.laser <= 30){
        color.x = 20.118;
        color.y = 0;
        color.z = 0;
    }
    else{
        color.x = 20.118;
        color.y = 1;
        color.z = 1;
    }
    
    up_displayText(ship_laser,length, &pos, &scale, font_assets, shader_program,0.002,&color);

}



