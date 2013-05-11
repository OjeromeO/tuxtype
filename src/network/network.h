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

//TODO: improve error handling (assert, global error variable, while(!shutdownserver), while(run), ...)

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
 * @brief   Send a message to a remote host.
 *
 * @details
 *      Messages transmission is done with 2 steps :
 *       - transmission of the message size
 *       - transmission of the real message
 *
 * @param sock  - TCP socket that will send the message.
 * @param buf   - Null-terminated message to send.
 * 
 * @return
 *      0 on success,
 *      -1 if an error occured,
 *      -2 if the remote host closed the connexion.
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

