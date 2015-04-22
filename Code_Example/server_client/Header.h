#ifndef server_Header_h
#define server_Header_h
#include <SDL2_net/SDL_net.h>





UDPsocket get_port_number(int ticker);
int init_sdl();
void recive(UDPpacket *packet,UDPsocket udpSocket);
void send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
UDPpacket *memory_check();
int client();
void terminate_process(UDPpacket *packet);




#endif
