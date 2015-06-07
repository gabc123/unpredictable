//
//  up_player.c
//  unpredictable_xcode
//
//  Created by o_0 on 2015-06-07.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#include "up_player.h"
#include "up_control_events.h"
#include "up_collisions.h"

// initialize the interface text
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


// moved to the server form here and down
//walled
//static void take_powerUp(struct up_player_stats *stats,int damage){
//
//
//    stats->health.current += damage;
//
//    if(stats->health.current >= stats->health.full){
//        stats->armor.current += stats->health.current - stats->health.full;
//        stats->health.current = stats->health.full;
//    }
//
//    if(stats->armor.current >= stats->armor.full){
//        stats->armor.current = stats->armor.full;
//    }
//
//}
//walled
//static void take_damage(struct up_player_stats *stats,int damage){
//
//    stats->armor.current -= damage;
//    if(stats->armor.current < 0){
//        stats->health.current += stats->armor.current;
//        stats->armor.current = 0;
//
//    }
//
//    stats->health.current = (stats->health.current > 0) ? stats->health.current : 0;
//
//}

/*
 fireMissile = 1,
 fireBullet,
 fireLaser
 
 
 */

//Wallid + Magnus
//static void ship_projectileHit(struct up_player_stats *player,struct up_shootingFlag *weapons,struct up_objectInfo *projectile)
//{
//    int damage = 0;
//    switch (projectile->projectile) {
//        case fireMissile:
//            damage = weapons->missileFlag.damage;
//            break;
//        case fireBullet:
//            damage = weapons->bulletFlag.damage;
//            break;
//        case fireLaser:
//            damage = weapons->laserFlag.damage;
//            break;
//
//        default:
//            damage = 0;
//            break;
//    }
//    take_damage(player,damage);
//
//}
//
//walled
// magnus included weapons so everything gets synced to the same state
void up_update_playerStats(struct up_allCollisions *collision,struct up_player_stats *player,struct up_shootingFlag *weapons, int playerId)                         //"Den checkar :P "
{
    
    // disabled on client;
    /*
     int i=0;
     int other_shipId;
     struct up_objectInfo *other_object = NULL;
     struct up_objectInfo *player_object = NULL;
     
     other_shipId = collision->shipShip[i].object2;
     player_object = up_unit_objAtIndex(up_ship_type, playerId);
     
     
     
     for(i=0; i<collision->nrShipEnviroment; i++){
     other_shipId = collision->shipShip[i].object2;
     other_object = up_unit_objAtIndex(up_environment_type, other_shipId);
     
     if (other_object == NULL) {
     continue;
     }
     if(collision->shipEnviroment[i].object1 == playerId){
     
     take_damage(player,7);// disabled in the client , runs on server now
     
     
     }
     }
     
     for(i=0; i<collision->nrShipShip; i++){
     
     if(collision->shipShip[i].object1 == playerId){
     other_shipId = collision->shipShip[i].object2;
     other_object = up_unit_objAtIndex(up_ship_type, other_shipId);
     
     if (other_object == NULL) {
     continue;
     }
     
     if(other_shipId !=  playerId ||
     other_object->modelId == player_object->modelId){
     take_damage(player,5);// disabled in the client , runs on server now
     
     }
     }
     }
     
     
     for(i=0; i<collision->nrProjectileShip; i++){
     if(collision->projectileShip[i].object2 == playerId){
     other_shipId = collision->projectileShip[i].object1;
     other_object = up_unit_objAtIndex(up_projectile_type, other_shipId);
     
     if (other_object == NULL) {
     continue;
     }
     if(other_object->modelId == 7){
     take_powerUp(player,20);
     }
     if (other_object->modelId ) {
     ship_projectileHit(player,weapons,other_object);
     }
     }
     }
     */
    // still in use by the client
    player->bullets.current = weapons->bulletFlag.ammunition;
    player->missile.current = weapons->missileFlag.ammunition;
    player->laser.current = weapons->laserFlag.ammunition;
}


