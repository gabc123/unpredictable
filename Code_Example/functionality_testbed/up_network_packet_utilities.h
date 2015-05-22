//
//  up_network_packet_utilities.h
//  up_server_part_xcode
//
//  Created by o_0 on 2015-05-17.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_NETWORK_PACKET_UTILITIES_H
#define UP_NETWORK_PACKET_UTILITIES_H

#include "up_ship.h"
#include "up_vertex.h"
#include "up_thread_utilities.h"

int up_network_action_packetEncode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 pos,float speed,
                                   float angle,float bankangle,int timestamp);

int up_network_action_packetDecode(struct objUpdateInformation *object,
                                   struct up_actionState *action,
                                   struct up_vec3 *pos,float *speed,
                                   float *angle,float *bankangle,int *timestamp);

#endif /* defined(__up_server_part_xcode__up_network_packet_utilities__) */
