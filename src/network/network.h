/*******************************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 ******************************************************************************/

/**
 * @file    network.h
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   General-purpose and generic network functions.
 */

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

/*
NOTES:
------

client names :
--------------
At first it could seem a good idea to put a default name like "Client <number>",
but if the dynamic allocation fails, what to do ? Its name would be NULL, and we
would have to check each time we want ot use it, if the name is != NULL,
displaying an other default name, maybe trying again to use a "Client <number>"
(with dynamic allocation, and it wouldn't solve the problem) ? Or something
else ? The client still wouldn't have a valid name... This mecanism wouldn't
prevent from always checking the client's name.
Then the solution is simple : the client's name remain NULL unless he chooses an
other one. And when we need a name for it, we use a get_client_name(int client)
that return the client name if != NULL, and a predefined name if == NULL. This
way, we are able to have a name for the client without failing.
=> Not a good idea to use a function : it would have to check if "client"
is a valid index, and if not, we still wouldn't have a name for the client.

network messages :
------------------
request   =>  COMMAND\0

response  =>  COMMAND\n
              info1\n
              info2\n
              ...
              infoN\0

*/

//TODO: don't forget to check frequently with valgrind if there are no memory leaks

//TODO: show on the proposal the tuxmath equivalents for my functions, and the differences

//TODO: no limit for MAX_SERVERS, MAX_CLIENTS, CLIENT_NAME, SERVER_NAME (realloc() if needed)

//TODO: check all the return to see if sometimes it's possible to return a succes for non-critical errors (malloc not working for the temporary name, ...)

//TODO: create a function for floor(log10(client+1))+1, like a numlen(int x) ?

//TODO: improve error handling with global error variable, while(!shutdownserver), while(run), ... ?

//TODO: instead of sending strings in messages, use a real structure for requests/responses ? with different fields (message type using an enum(), length, ...)

//TODO: improve the structure => compare with network.c/h and server.c/h
// => a module for the client (with internal variables, ...), an other for the
//    server (with internal variables, ...), and shared functions in a
//    network.c/h module (its functions are uppercase because generic)
// => look at how it is done in tuxmaths and discuss this idea on the mailing-list
// => whenever possible, use/create a function of network.c/h, with a wrapper if needed ; not possible if
//    it's a server-specific function (send_questions, end_game, ...) or a
//    client-specific function
// => client and server modules functions will probably be all lowercase except
//    RunServer/Client (and maybe some others like DetectServer?) ; even if tux maths uses
//    several functions for client instead of one RunClient() (LAN_xxx(),
//    Connecttoservers, Pregame, ...), or maybe it needs more explicit function names ?
//    => because of the menu and UI part... so it's OK
// => client/server main will become RunServer and RunClient



//TODO: move "set" to be the return value ?
//TODO: make the same function with a variable argument list ?
/**
 * @brief   Allocate and initialize a new socket set.
 * 
 * @param set           - Socket set that will be filled.
 * @param tcpsockets    - Array of TCP sockets to add to the new socket set.
 * @param tcpcount      - Count of TCP sockets to add to the new socket set.
 * @param udpsockets    - Array of UDP sockets to add to the new socket set.
 * @param udpcount      - Count of UDP sockets to add to the new socket set.
 * 
 * @return
 *      0 on success,
 *      -1 if an error occured.
 */
extern int CreateSocketSet(SDLNet_SocketSet * set, TCPsocket tcpsockets[], int tcpcount, UDPsocket udpsockets[], int udpcount);

/**
 * @brief   Send a string to a remote host.
 *
 * @details
 *      Messages transmission is done with 2 steps :
 *       - transmission of the message size
 *       - transmission of the real message
 *
 * @param sock  - TCP socket that will send the message.
 * @param buf   - Null-terminated string to send.
 * 
 * @return
 *      0 on success,
 *      -1 if an error occured,
 *
 */
extern int SendMessage(TCPsocket sock, char * buf);

//TODO: create a tcp_recv_msg() and a udp_recv_msg() and then a wrapper RecvMessage(, , flags) with UDP_MSG|TCP_MSG ?
/**
 * @brief   Receive a message from a remote host in a buffer it allocates.
 *
 * @details
 *      Messages transmission is done with 2 steps :
 *       - transmission of the message size
 *       - transmission of the real message
 *
 * @param sock  - TCP socket that will receive the message.
 * @param buf   - Buffer where the message will be stored.
 * 
 * @return
 *      0 on success,
 *      -1 if an error occured,
 *      -2 if the remote host closed the connexion.
 *
 */
extern int RecvMessage(TCPsocket sock, char ** buf);



#endif

