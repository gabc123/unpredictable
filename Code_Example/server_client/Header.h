#ifndef server_Header_h
#define server_Header_h
#include <SDL2_net/SDL_net.h>

//both server and client chare these funktion
struct pthread_listen_datapipe{
    UDPsocket udpSocket;
    UDPpacket *packet;
   // char *buffer;

};
typedef struct pthread_listen_datapipe Pthread_listen_datapipe;




UDPsocket get_port_number(int ticker);
int init_sdl();
UDPpacket *memory_check();
void terminate_process(UDPpacket *packet,SDLNet_SocketSet *set);
void error_messages();
SDLNet_SocketSet allo_addsock(Pthread_listen_datapipe *p);



//server using these funktions
int server_recive_data(void*parameter);
void server_send_data(Pthread_listen_datapipe *listen_pipe);



//client using these funktions
int client_recive_data(void *parameters);
void client_send_data(UDPpacket *packet,UDPsocket *udpSocket,IPaddress addr);


#endif
