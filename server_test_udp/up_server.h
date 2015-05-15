//
//  up_server.h
//  server_test_udp
//
//  Created by o_0 on 2015-05-10.
//  Copyright (c) 2015 o_0. All rights reserved.
//

#ifndef UP_SERVER_H
#define UP_SERVER_H
struct internal_server_state;

struct internal_server_state *up_server_startup();
void up_server_run(struct internal_server_state *server_state);
void up_server_shutdown_cleanup(struct internal_server_state *server_state);
#endif /* defined(__server_test_udp__up_server__) */
