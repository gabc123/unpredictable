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
#include <math.h>
#include <stdlib.h>
#include "up_error.h"

static float up_distance_calc(struct up_vec3 pos1, struct up_vec3 pos2)
{
    float x =  (pos1.x - pos2.x);
    float y =  (pos1.y - pos2.y);
    float z =  (pos1.z - pos2.z);
    
    return sqrtf(x*x + y*y + z*z);
}

float up_radar_search(struct up_interface_game *interface,struct up_assets *assets,struct up_objectInfo *ship)
{
    int numShip = 0;
    struct up_objectInfo *shipArray = up_unit_getAllObj(up_ship_type, &numShip);
    
    int i = 0;
    float minDistance = 2000;
    float tmpDistance = 2000;
    int closesShipId = 0;
    for (i = 0; i < numShip; i++) {
        if (!up_unit_isActive(&shipArray[i])) {
            continue;
        }
        // if we are there
        if (shipArray[i].objectId.idx == ship->objectId.idx) {
            continue;
        }
        
        tmpDistance = up_distance_calc(shipArray[i].pos,ship->pos);
        if (tmpDistance < minDistance) {
            minDistance = tmpDistance;
            closesShipId = i;
        }
        
    }
    float dx = shipArray[closesShipId].pos.x - ship->pos.x;
    float dy = shipArray[closesShipId].pos.y - ship->pos.y;
    
    // fallback to prevent divide by zero
    if (dy == 0) {
        return interface->radar.enemyAngle;
    }
    
    float enemyAngele = atanf(dx/dy);
    
    float angleToEnemy = -enemyAngele + ship->angle;
    
    interface->radar.enemyAngle = angleToEnemy;
    
    return angleToEnemy;
}


static struct up_interface_bar healthBar_creation(struct up_interface_inventory inventory,
                                                  struct up_vec3 pos, struct up_vec3 scale, int fullModelId, int emptyModelId){
    
    struct up_interface_bar bar = {0};
    bar.level = (float)inventory.current/inventory.full;
    bar.pos = pos;
    bar.scale = scale;
    bar.fullModelId = fullModelId;
    bar.emptyModelId = emptyModelId;

    return bar;
}

struct up_vec3 up_set_vec3(float x, float y, float z){
    
    struct up_vec3 vec = {x,y,z};
    
    return vec;
}

static void up_interface_placement(FILE *fp,struct up_interface_inventory *tmp){

    fscanf(fp,"%f/%f/%f %f/%f/%f %f/%f/%f", &tmp->pos.x, &tmp->pos.y, &tmp->pos.z,
           &tmp->scale.x, &tmp->scale.y, &tmp->scale.z,&tmp->color.x, &tmp->color.y, &tmp->color.z);
}

void up_interface_creation(struct up_interface_game *interface, struct up_player_stats *player){
    
    interface->playerStats.health.full = 100;
    interface->playerStats.health.current = 100;
    interface->playerStats.armor.full = 100;
    interface->playerStats.armor.current = 100;

    
    interface->playerStats.bullets.full = player->bullets.full;
    interface->playerStats.bullets.current = player->bullets.current;
    interface->playerStats.missile.full = player->missile.full;
    interface->playerStats.missile.current = player->missile.current;
    interface->playerStats.laser.full = player->laser.full;
    interface->playerStats.laser.current = player->laser.current;
    
    FILE *fp = fopen("interface", "r");
    if (fp == NULL) {
        UP_ERROR_MSG("Interface file not found");
        return;
    }
    // for text
    up_interface_placement(fp,&interface->playerStats.health);
    up_interface_placement(fp,&interface->playerStats.armor);
    up_interface_placement(fp,&interface->playerStats.bullets);
    up_interface_placement(fp,&interface->playerStats.missile);
    up_interface_placement(fp,&interface->playerStats.laser);
    
    struct up_interface_inventory radar_tmp = {0};
    up_interface_placement(fp,&radar_tmp);
    interface->radar.pos = radar_tmp.pos;
    interface->radar.scale = radar_tmp.scale;
    interface->radar.enemyAngle = 0.0;
    interface->radar.modelId = 14;
    
    fclose(fp);
    
    struct up_vec3 scale = {0.2,3.0,0.3};
    struct up_vec3 pos = interface->playerStats.health.pos;
    pos.x += 0.3;
    
    int fullModelId = 5;
    int emptyModelId = 6;
    interface->health = healthBar_creation(interface->playerStats.health, pos, scale, fullModelId, emptyModelId);
    
    
    pos = interface->playerStats.armor.pos;
    pos.x += 0.3;
    interface->armor = healthBar_creation(interface->playerStats.health, pos, scale, fullModelId, emptyModelId);

    
    
}


void up_player_setup(struct up_player_stats *player, struct up_shootingFlag weapons){
    player->health.current = 100;
    player->health.full = 100;
    player->armor.current = 100;
    player->armor.full = 100;
    
    player->bullets.full = weapons.bulletFlag.ammunition;
    player->bullets.current = weapons.bulletFlag.ammunition;
    player->missile.full = weapons.missileFlag.ammunition;
    player->missile.current = weapons.missileFlag.ammunition;
    player->laser.full = weapons.laserFlag.ammunition;
    player->laser.current = weapons.laserFlag.ammunition;
    
}


void up_interface_healthBar(struct up_assets *assets, struct up_interface_bar *bar, struct shader_module *shader_prog){
    
    up_matrix4_t fullTransform = up_matrix4identity();
    up_matrix4_t emptyTransform = up_matrix4identity();
    struct up_vec3 rot = {0};
    
    
    UP_shader_bind(shader_prog);
    
    int fullModelId = bar->emptyModelId;
    int emptyModelId = bar->fullModelId;
    
    if (fullModelId >= assets->numobjects) {
        fullModelId = 0;
    }
    if (emptyModelId >= assets->numobjects) {
        emptyModelId = 0;
    }
    
    struct up_texture_data *fullTexture = &assets->textureArray[fullModelId];
    struct up_texture_data *emptyTexture = &assets->textureArray[emptyModelId];
    
    struct up_mesh *fullMesh = &assets->meshArray[fullModelId];
    struct up_mesh *emptyMesh = &assets->meshArray[emptyModelId];
    
    struct up_vec3 scale = bar->scale;
    
    struct up_vec3 pos = bar->pos;
    pos.z += 0.01;
    
    scale.x *= bar->level;

    up_matrixModel(&fullTransform, &bar->pos, &rot, &scale);
    up_matrixModel(&emptyTransform, &pos, &rot, &bar->scale);
    
    up_texture_bind(emptyTexture, 0);
    UP_shader_update(shader_prog,&emptyTransform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh
    up_draw_mesh(emptyMesh);
    
    up_texture_bind(fullTexture, 0);
    UP_shader_update(shader_prog,&fullTransform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh
    up_draw_mesh(fullMesh);
}

void up_interface_update(struct up_interface_game *inteface, struct up_player_stats *player){
    
    inteface->health.level = (float)player->health.current / player->health.full;
    inteface->armor.level = (float)player->armor.current / player->armor.full;
    
    inteface->playerStats.health.current = player->health.current;
    inteface->playerStats.armor.current = player->armor.current;
    inteface->playerStats.bullets.current = player->bullets.current;
    inteface->playerStats.missile.current = player->missile.current;
    inteface->playerStats.laser.current = player->laser.current;
    
}
void up_inventory_text(struct up_interface_inventory *inventory,struct up_font_assets *font_assets,struct shader_module *shader_program){
    
    char text[50];
    
    sprintf(text,"%d/%d", inventory->
            current,inventory->full);
    
    int length = (int)strlen(text);
    
    up_displayText(text,length, &inventory->pos, &inventory->scale, font_assets, shader_program,0.02,&inventory->color);
    
}

void up_interface_playerStats(struct up_interface_stats *stats,struct up_font_assets *font_assets,struct shader_module *shader_program){
    
    up_inventory_text(&stats->health, font_assets, shader_program);
    up_inventory_text(&stats->armor, font_assets, shader_program);
    up_inventory_text(&stats->bullets, font_assets, shader_program);
    up_inventory_text(&stats->missile, font_assets, shader_program);
    up_inventory_text(&stats->laser, font_assets, shader_program);
    
}


static void up_render_symbols(struct up_assets *assets,struct shader_module *shader_program,struct up_interface_symbols *symbolArray,int length){
    
    up_matrix4_t transform = up_matrix4identity();
    struct up_vec3 rot = {0};
    int modelId = 0;
    struct up_texture_data *texture = NULL;
    struct up_mesh *mesh;
    
    UP_shader_bind(shader_program);
    int i=0;
    for(i=0; i<length; i++){
        
        modelId = symbolArray[i].modelId;
        if (modelId >= assets->numobjects) {
            modelId = 0;
        }
        texture = &assets->textureArray[modelId];
        mesh = &assets->meshArray[modelId];
        
        up_matrixModel(&transform, &symbolArray[i].pos, &rot, &symbolArray[i].scale);
        
        up_texture_bind(texture, 0);
        
        UP_shader_update(shader_program,&transform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh
        
        up_draw_mesh(mesh);
    }
    
    
}

void up_interface_renderRadar(struct up_assets *assets ,struct up_interface_radar *radar,struct shader_module *shader_program)
{
    up_matrix4_t transform = up_matrix4identity();
    struct up_vec3 rot = {0};
    struct up_vec3 pos = radar->pos;
    
    // this gives the placment of the radar bar relative the pos.
    pos.x += sinf(radar->enemyAngle) *radar->scale.x;
    
    int modelId = radar->modelId;
    if (modelId >= assets->numobjects) {
        modelId = 0;
    }
    
    struct up_texture_data *texture = &assets->textureArray[modelId];
    struct up_mesh *mesh = &assets->meshArray[modelId];
    struct up_vec3 *scale = &assets->scaleArray[modelId];
    
    up_matrixModel(&transform, &pos, &rot, scale);
    
    up_texture_bind(texture, 0);
    
    UP_shader_update(shader_program,&transform);    // this uploads the transform to the gpu, and will now be applayed to up_draw_mesh
    
    up_draw_mesh(mesh);
    

    
}


void up_interface_gamePlay(struct up_assets *assets ,struct up_font_assets *font_assets,
                           struct shader_module *shader_program,struct up_interface_game *interface){
    
    
    up_render_symbols(assets,shader_program,interface->symbolArray,interface->countSymbol);

    up_interface_playerStats(&interface->playerStats,font_assets,shader_program);

    up_interface_healthBar(assets, &interface->health, shader_program);
    
    up_interface_healthBar(assets, &interface->armor, shader_program);
    
    up_interface_renderRadar(assets , &interface->radar, shader_program);
}





