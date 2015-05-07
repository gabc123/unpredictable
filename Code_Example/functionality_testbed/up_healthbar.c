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



int healthbar_creation()
{
    
    int index;
    struct up_objectInfo healthBar = up_asset_createObjFromId(0);
    
    //healthBar.pos = ;
    
    index = up_unit_add(up_others_type,healthBar);

    
    
    
    return index;
    
    
}


void moveHealthBar(struct up_objectInfo *ship,int index)
{
    
    struct up_objectInfo *healthbar = up_unit_objAtIndex(up_others_type,index);
    
    healthbar->pos.x = ship->pos.x;
    healthbar->pos.y = ship->pos.y+ 20;
    healthbar->pos.z = ship->pos.z;
    
    
}



