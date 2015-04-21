#ifndef server_Header_h
#define server_Header_h
#include <SDL2_net/SDL_net.h>






void get_port_number(UDPsocket udpSocket,int ticker);
int init_sdl();
void recive(UDPpacket *packet,UDPsocket *udpSocket);
void send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
int malloc_memory(UDPpacket *packet);
int client();
void terminate_process(UDPpacket *packet);




#endif
