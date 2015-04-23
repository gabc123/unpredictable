#ifndef server_Header_h
#define server_Header_h
#include <SDL2_net/SDL_net.h>


struct pthread_listen_datapipe{
    UDPsocket udpSocket;                               //structure that contains two diffrent structures
    UDPpacket *packet;
    SDLNet_SocketSet set;
};
typedef struct pthread_listen_datapipe Pthread_listen_datapipe;

UDPsocket get_port_number(int ticker);
int init_sdl();
int recive_server_data(void*parameter);
void send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);
UDPpacket *memory_check();
int client();
void terminate_process(UDPpacket *packet,SDLNet_SocketSet *set);
void error_messages();

void server_send_data(Pthread_listen_datapipe *listen_pipe);
int client_recive_data(void *parameters);


#endif
