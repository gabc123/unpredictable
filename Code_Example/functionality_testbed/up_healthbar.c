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

    struct up_objectInfo healthBarGreen = up_asset_createObjFromId(0);
    struct up_objectInfo healthBarRed = up_asset_createObjFromId(0);

    healthBarGreen.scale.x = 1;
    healthBarGreen.scale.x = 1;
    healthBarGreen.scale.x = 1;

    healthBarRed.scale.x = 1;
    healthBarRed.scale.y = 1;
    healthBarRed.scale.z = 1;

    green_and_red.greenIndex = up_unit_add(up_others_type,healthBarGreen);
    green_and_red.redIndex = up_unit_add(up_others_type,healthBarGreen);




    return green_and_red;


}


void moveHealthBar(int ship_id,up_health_bar_t healing)
{
    struct up_objectInfo *ship_pos = up_unit_objAtIndex(up_ship_type,ship_id);

    struct up_objectInfo *healthGreen = up_unit_objAtIndex(up_others_type,healing.greenIndex);
    struct up_objectInfo *healthRed = up_unit_objAtIndex(up_others_type,healing.redIndex);

    healthGreen->pos.x = ship_pos->pos.x;
    healthGreen->pos.y = ship_pos->pos.y+5;
    healthGreen->pos.z = ship_pos->pos.z;

    healthRed->pos.x = ship_pos->pos.x;
    healthRed->pos.y = ship_pos->pos.y+5;
    healthRed->pos.z = ship_pos->pos.z;


}



