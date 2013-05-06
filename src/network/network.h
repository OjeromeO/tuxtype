#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#define MAX_SERVERS 50
#define MAX_CLIENTS 4
#define DEFAULT_PORT 1337
#define CLIENT_DATA "TuxMaths client"
#define SERVER_DATA "TuxMaths server"
#define MSG_MAXLEN 1024

// contains general-purpose, generic network functions

//TODO: client/server main will become RunServer and RunClient

//TODO: write the doxygen documentation

//TODO: improve the structure => compare with network.c/h and server.c/h
// => a module for the client (with internal variables, ...), an other for the
//    server (with internal variables, ...), and shared functions in a
//    network.c/h module (its functions are uppercase because generic)
// => look at how it is done in tuxmaths and discuss this idea with members of tux4kids on the mailing-list
// => whenever possible, use/create a function of network.c/h ; not possible if
//    it's a server-specific function (send_questions, end_game, ...) or a
//    client-specific function
// => client and server modules functions will probably be all lowercase except
//    RunServer/Client (and maybe some others like DetectServer?) ; even if tux maths uses
//    several functions for client instead of one RunClient() (LAN_xxx(),
//    Connecttoservers, Pregame, ...), or maybe it needs more explicit function names ?
//    => because of the menu and UI part... so it's OK
/*
examples:
RecvMessage in network
CreateSocketSet in network
one create_socket_set in client and one in server, each wrapper calling CreateSocketSet
*/

extern int CreateSocketSet(SDLNet_SocketSet * set, TCPsocket tcpsockets[], int tcpcount, UDPsocket udpsockets[], int udpcount);
extern int RecvMessage(TCPsocket sock, char ** buf);
//int AddClient(TCPsocket * socket, int * num_clients, TCPsocket clients[], SDLNet_SocketSet socketset);


//int create_socketset(void);





#endif

