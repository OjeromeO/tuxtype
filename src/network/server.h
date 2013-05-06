#ifndef _SERVER_H_
#define _SERVER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>



extern SDLNet_SocketSet set;
extern TCPsocket listeningtcpsock;
extern TCPsocket clients[];
extern int num_clients;



#endif

