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
 * @file    network.c
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   General-purpose generic network functions.
 */

#include "network.h"



int CreateSocketSet(SDLNet_SocketSet * set, TCPsocket tcpsockets[], int tcpcount, UDPsocket udpsockets[], int udpcount)
{
    int i = 0;
    int ret = 0;
    
    if (set == NULL
     || ((tcpsockets == NULL || tcpcount <= 0) && (udpsockets == NULL || udpcount <= 0)))
    {
        fprintf(stderr, "create_socketset: Invalid argument(s).\n");
        return -1;
    }
    
    *set = SDLNet_AllocSocketSet(tcpcount+udpcount);
    if (*set == NULL)
    {
        fprintf(stderr, "create_socketset: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return -1;
    }
    
    for(i=0;i<tcpcount;i++)
    {
        if (tcpsockets[i] == NULL)
            continue;
        
        ret = SDLNet_TCP_AddSocket(*set, tcpsockets[i]);
        if (ret == -1)
        {
            fprintf(stderr, "create_socketset: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
            SDLNet_FreeSocketSet(*set);
            return -1;
        }
    }
    
    for(i=0;i<udpcount;i++)
    {
        if (udpsockets[i] == NULL)
            continue;
        
        ret = SDLNet_TCP_AddSocket(*set, udpsockets[i]);
        if (ret == -1)
        {
            fprintf(stderr, "create_socketset: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
            SDLNet_FreeSocketSet(*set);
            return -1;
        }
    }
    
    return 0;
}

int RecvMessage(TCPsocket sock, char ** buf)
{
    Uint32 len = 0;
    Uint32 buflen = 0;
    int ret = 0;
    
    if (sock == NULL || buf == NULL)
    {
        fprintf(stderr, "RecvMessage: Invalid argument(s).\n");
        return -1;
    }
    
    ret = SDLNet_TCP_Recv(sock, &buflen, sizeof(buflen));
    if (ret <= 0)
    {
        if (SDLNet_GetError() != NULL && strlen(SDLNet_GetError()) != 0)
        {
            fprintf(stderr, "RecvMessage: SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            *buf = NULL;
            return -1;
        }
        else
        {
            fprintf(stderr, "RecvMessage: Connexion closed by the other side.\n");
            *buf = NULL;
            return -2;
        }
    }
    
    len = SDLNet_Read32(&buflen);
    if (len <= 0)
    {
        fprintf(stderr, "RecvMessage: Zero length message from the client...\n");
        *buf = NULL;
        return -1;
    }
    
    *buf = malloc(len * sizeof(char));
    memset(*buf, '\0', len);
    if (*buf == NULL)
    {
        fprintf(stderr, "RecvMessage: malloc: Can't allocate memory for a message\n");
        return -1;
    }
    
    ret = SDLNet_TCP_Recv(sock, *buf, len);
    if (ret <= 0)
    {
        if (SDLNet_GetError() != NULL && strlen(SDLNet_GetError()) != 0)
        {
            fprintf(stderr, "RecvMessage: SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
            free(*buf);
            *buf = NULL;
            return -1;
        }
        else
        {
            fprintf(stderr, "RecvMessage: Connexion closed by the other side.\n");
            free(*buf);
            *buf = NULL;
            return -2;
        }
    }
    
    return 0;
}

int SendMessage(TCPsocket sock, char * buf)
{
    Uint32 len = 0;
    Uint32 buflen = 0;
    int ret = 0;
    
    if (sock == NULL || buf == NULL)
    {
        fprintf(stderr, "SendMessage: Invalid argument(s).\n");
        return -1;
    }
    
    buflen = strlen(buf)+1;
    SDLNet_Write32(buflen, &len);
    
    ret = SDLNet_TCP_Send(sock, &len, sizeof(len));
    if (ret < (int)sizeof(len))
    {
        fprintf(stderr, "SendMessage: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return -1;
    }
    
    ret = SDLNet_TCP_Send(sock, buf, strlen(buf)+1);
    if (ret < (int)strlen(buf)+1)
    {
        fprintf(stderr, "SendMessage: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
        return -1;
    }
    
    return 0;
}

