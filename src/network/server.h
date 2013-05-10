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
 * @file    server.h
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   Server management module.
 */

#ifndef _SERVER_H_
#define _SERVER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>



/**
 * Socket set the server has to watch.
 */
extern SDLNet_SocketSet set;

/**
 * TCP socket to listen and accept incoming connections.
 */
extern TCPsocket listeningtcpsock;

/**
 * TCP sockets binded to clients.
 */
extern TCPsocket clients[];

/**
 * Count of clients.
 */
extern int num_clients;



#endif

