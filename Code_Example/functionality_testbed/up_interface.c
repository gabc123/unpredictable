#include "up_interface.h"
#include "up_graphics_update.h"
#include "up_render_engine.h"
#include "up_error.h"

// Callculate the colsiest ship around
// Magnus
static float up_distance_calc(struct up_vec3 pos1, struct up_vec3 pos2)
{
    float x =  (pos1.x - pos2.x);
    float y =  (pos1.y - pos2.y);
    float z =  (pos1.z - pos2.z);
    
    return sqrtf(x*x + y*y + z*z);
}
// searching for ships around
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
    
    float enemyAngele = asinf(dx/minDistance);
    
    float angleToEnemy = -(ship->angle + enemyAngele);
    
    interface->radar.enemyAngle = angleToEnemy;
    
    return angleToEnemy;
}

// walid
// creates the interface tools to apper on the screen
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

    interface->countSymbol = 5;

    interface->symbolArray[0].modelId = 9;
    interface->symbolArray[0].pos = interface->playerStats.health.pos;
    interface->symbolArray[0].pos.x -=0.5;
    interface->symbolArray[0].scale = interface->playerStats.health.scale;
    interface->symbolArray[0].scale.x = 0.03;
    interface->symbolArray[0].scale.y = 0.03;
    interface->symbolArray[0].scale.z = 0.03;
    

    interface->symbolArray[1].modelId = 10;
    interface->symbolArray[1].pos = interface->playerStats.armor.pos;
    interface->symbolArray[1].pos.x -=0.5;
    interface->symbolArray[1].scale = interface->playerStats.armor.scale;
    interface->symbolArray[1].scale.x = 0.015;
    interface->symbolArray[1].scale.y = 0.015;
    interface->symbolArray[1].scale.z = 0.015;
    
    struct up_interface_inventory radar_tmp = {0};
    up_interface_placement(fp,&radar_tmp);
    interface->radar.pos = radar_tmp.pos;
    interface->radar.scale = radar_tmp.scale;
    interface->radar.enemyAngle = 0.0;
    interface->radar.modelId = 14;
    
    fclose(fp);
    
    struct up_vec3 scale = {0.2,3.0,0.3};
    struct up_vec3 pos = interface->playerStats.health.pos;
    pos.x -= 0.45;
    
    int fullModelId = 5;
    int emptyModelId = 6;
    interface->health = healthBar_creation(interface->playerStats.health, pos, scale, fullModelId, emptyModelId);
    
    fullModelId = 17;
    emptyModelId = 16;
    pos = interface->playerStats.armor.pos;
    pos.x -= 0.45;
    interface->armor = healthBar_creation(interface->playerStats.health, pos, scale, fullModelId, emptyModelId);


    
}


// transform matrix for the model and texture. load it up to the GPU and render.
void up_interface_healthBar(struct up_assets *assets, struct up_interface_bar *bar, struct up_shader_module *shader_prog){
    
    up_matrix4_t fullTransform = up_matrix4identity();
    up_matrix4_t emptyTransform = up_matrix4identity();
    struct up_vec3 rot = {0};
    
    
    up_shader_bind(shader_prog);
    
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
    up_shader_update(shader_prog,&emptyTransform);    // this uploads the transform to the gpu, and will now be applayed to up_mesh_draw
    up_mesh_draw(emptyMesh);
    
    up_texture_bind(fullTexture, 0);
    up_shader_update(shader_prog,&fullTransform);    // this uploads the transform to the gpu, and will now be applayed to up_mesh_draw
    up_mesh_draw(fullMesh);
}

//update the initialize values.
void up_interface_update(struct up_interface_game *inteface, struct up_player_stats *player){
    
    inteface->health.level = (float)player->health.current / player->health.full;
    inteface->armor.level = (float)player->armor.current / player->armor.full;
    
    inteface->playerStats.health.current = player->health.current;
    inteface->playerStats.armor.current = player->armor.current;
    inteface->playerStats.bullets.current = player->bullets.current;
    inteface->playerStats.missile.current = player->missile.current;
    inteface->playerStats.laser.current = player->laser.current;
    
}

// takes inventory current and full and puts it into text and sends it to display
void up_inventory_text(struct up_interface_inventory *inventory,struct up_font_assets *font_assets,struct up_shader_module *shader_program){
    
    char text[50];
    
    sprintf(text,"%d/%d", inventory->
            current,inventory->full);
    
    int length = (int)strlen(text);
    
    up_displayText(text,length, &inventory->pos, &inventory->scale, font_assets, shader_program,0.02,&inventory->color);
    
}
// uses the up_inventory funktion to show the stats on the interface.
void up_interface_playerStats(struct up_interface_stats *stats,struct up_font_assets *font_assets,struct up_shader_module *shader_program){
    
    up_inventory_text(&stats->health, font_assets, shader_program);
    up_inventory_text(&stats->armor, font_assets, shader_program);
    up_inventory_text(&stats->bullets, font_assets, shader_program);
    up_inventory_text(&stats->missile, font_assets, shader_program);
    up_inventory_text(&stats->laser, font_assets, shader_program);
    
}

// Render the the existing model that is uploaded in the GPU. this funktion updates and send the treanformatrix for the symbols(interface).
static void up_render_symbols(struct up_assets *assets,struct up_shader_module *shader_program,struct up_interface_symbols *symbolArray,int length){
    
    up_matrix4_t transform = up_matrix4identity();
    struct up_vec3 rot = {0};
    int modelId = 0;
    struct up_texture_data *texture = NULL;
    struct up_mesh *mesh;
    
    up_shader_bind(shader_program);
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
        
        up_shader_update(shader_program,&transform);    // this uploads the transform to the gpu, and will now be applayed to up_mesh_draw
        
        up_mesh_draw(mesh);
    }
    
    
}
//  Render the the existing model that is uploaded in the GPU. this funktion updates and send the treanformatrix for the radar.
void up_interface_renderRadar(struct up_assets *assets ,struct up_interface_radar *radar,struct up_shader_module *shader_program)
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
    
    up_shader_update(shader_program,&transform);    // this uploads the transform to the gpu, and will now be applayed to up_mesh_draw
    
    up_mesh_draw(mesh);
    
}

// main module for up_healthbar.c. everyting in the funktion is linkt to this one that is linked to the game loop
void up_interface_gamePlay(struct up_assets *assets ,struct up_font_assets *font_assets,
                           struct up_shader_module *shader_program,struct up_interface_game *interface){
    
    
    up_render_symbols(assets,shader_program,interface->symbolArray,interface->countSymbol);

    up_interface_playerStats(&interface->playerStats,font_assets,shader_program);

    up_interface_healthBar(assets, &interface->health, shader_program);
    
    up_interface_healthBar(assets, &interface->armor, shader_program);
    
    up_interface_renderRadar(assets , &interface->radar, shader_program);
}





