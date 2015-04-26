#ifndef __client__client_funk__
#define __client__client_funk__
#include <SDL2_net/SDL_net.h>




//client using these funktions
int client_recive_data(void *parameters);
void client_send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
int check_incoming_data(SDLNet_SocketSet set);







#endif
