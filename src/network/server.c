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
 * @file    server.c
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   Server management module.
 */

#include "network.h"
#include "server.h"



SDLNet_SocketSet set = NULL;
TCPsocket listeningtcpsock = NULL;
TCPsocket clients[MAX_CLIENTS] = {0};
int num_clients = 0;



static int setup_server(int argc, char ** argv);
static int add_client(TCPsocket tmpsock);
static int remove_client(int i);
static int handle_client_msg(char * msg);
static void cleanup_server(void);

//TODO: add commands mangement (quit, connected clients, setname, ...) and response from the server

int main(int argc, char ** argv)
{
    int i = 0;
    int ret = 0;
    char * msg = NULL;
    //UDPsocket listeningudpsock = NULL;
    TCPsocket tmpsock = NULL;
    //UDPpacket * out = NULL;
    //UDPpacket * in = NULL;
    
    ret = setup_server(argc, argv);
    if (ret != 0)
    {
        cleanup_server();
        return EXIT_FAILURE;
    }
    
    //TODO: creation of the listening UDP socket
    /*in = SDLNet_AllocPacket(strlen(CLIENT_DATA)+1);
    if (in == NULL)
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(listeningtcpsock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    out = SDLNet_AllocPacket(strlen(SERVER_DATA)+1);
    if (out == NULL)
    {
        SDLNet_FreePacket(in);
        SDLNet_TCP_Close(listeningtcpsock);
        SDLNet_Quit();
        SDL_Quit();
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return EXIT_FAILURE;
    }
    
    listeningudpsock = SDLNet_UDP_Open(DEFAULT_PORT);
    if (listeningudpsock == NULL)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_FreePacket(in);
        SDLNet_FreePacket(out);
        SDLNet_TCP_Close(listeningtcpsock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }*/
    
    while (1)
    {
        /*in->data = NULL;
        in->address.host = 0;
        in->address.port = 0;
        out->data = NULL;
        out->address.host = 0;
        out->address.port = 0;
        
        // check UDP broadcasts
        
        ret = SDLNet_UDP_Recv(listeningudpsock, in);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_UDP_Recv: %s\n", SDLNet_GetError());
            SDLNet_FreePacket(in);
            SDLNet_FreePacket(out);
            for (i=0;i<num_clients;i++)
                SDLNet_TCP_Close(clients[i]);
            SDLNet_UDP_Close(listeningudpsock);
            SDLNet_TCP_Close(listeningtcpsock);
            SDLNet_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        if (ret == 1)
        {
            if (strncmp((char*)in->data, CLIENT_DATA, strlen(CLIENT_DATA)) == 0)
            {
                fprintf(stderr, "recu: %s from %d:%d\n", in->data, in->address.host, in->address.port);
                
                out->address.host = in->address.host;
                out->address.port = in->address.port;
                sprintf((char *)out->data, SERVER_DATA);
                out->len = strlen(SERVER_DATA) + 1;
                
                ret = SDLNet_UDP_Send(listeningudpsock, -1, out);
                if (ret == 0)
                {
                    fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
                    SDLNet_FreePacket(in);
                    SDLNet_FreePacket(out);
                    for (i=0;i<num_clients;i++)
                        SDLNet_TCP_Close(clients[i]);
                    SDLNet_UDP_Close(listeningudpsock);
                    SDLNet_TCP_Close(listeningtcpsock);
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }
            }
            else
            {
                fprintf(stderr, "Not a tuxtype packet\n");
            }
        }*/
        
        // check sockets activity
        
        ret = SDLNet_CheckSockets(set, -1);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            perror("SDLNet_CheckSockets");
            cleanup_server();
            return EXIT_FAILURE;
        }
        
        if (ret > 0)
        {
            // accept incoming TCP connections
            
            if (SDLNet_SocketReady(listeningtcpsock))
            {
                tmpsock = SDLNet_TCP_Accept(listeningtcpsock);
                if (tmpsock == NULL)
                {
                    fprintf(stderr, "SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
                    cleanup_server();
                    return EXIT_FAILURE;
                }
                
                switch (add_client(tmpsock))
                {
                    case 0:     fprintf(stderr, "========> Client %d connected\n", num_clients);
                                break;
                                
                    case -1:    SDLNet_TCP_Close(tmpsock);
                                cleanup_server();
                                return EXIT_FAILURE;
                                break;
                                
                    case -2:    SDLNet_TCP_Close(tmpsock);
                                break;
                }
            }
            
            // receive clients messages
            
            for (i=0;i<num_clients;i++)
            {
                if (SDLNet_SocketReady(clients[i]))
                {
                    switch (RecvMessage(clients[i], &msg))
                    {
                        case 0:     fprintf(stderr, "Client %d: %s\n", i+1, msg);
                                    //TODO: manage clients commands here
                                    ret = handle_client_msg(msg);
                                    if (ret == -1)
                                    {
                                        free(msg);
                                        cleanup_server();
                                        return EXIT_FAILURE;
                                    }
                                    
                                    if (ret == -2)
                                    {
                                        fprintf(stderr, "Server shutdown.\n");
                                        free(msg);
                                        cleanup_server();
                                        return EXIT_SUCCESS;
                                    }
                                    
                                    free(msg);
                                    break;
                                    
                        case -1:    cleanup_server();
                                    return EXIT_FAILURE;
                                    break;
                                    
                        case -2:    fprintf(stderr, "========> Client %d disconnected\n", i+1);
                                    ret = remove_client(i);
                                    if (ret != 0)
                                    {
                                        cleanup_server();
                                        return EXIT_FAILURE;
                                    }
                                    break;
                    }
                }
            }
        }
    }
    
    // cleanup    
    
    //SDLNet_FreePacket(in);
    //SDLNet_FreePacket(out);
    //SDLNet_UDP_Close(listeningudpsock);
    
    cleanup_server();
    
    return EXIT_SUCCESS;
}

int setup_server(int argc, char ** argv)
{
    int ret = 0;
    IPaddress ip = {0, 0};
    
    // handle arguments
    
    if (argc != 2)
    {
        fprintf(stderr, "%s: Invalid arguments\n", argv[0]);
        fprintf(stderr, "    Usage: %s port\n", argv[0]);
        return -1;
    }
    
    // SDL initializations
    
    ret = SDL_Init(0);
    if (ret != 0)
    {
        fprintf(stderr, "setup_server: SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
    
    ret = SDLNet_Init();
    if (ret != 0)
    {
        fprintf(stderr, "setup_server: SDLNet_Init: %s\n", SDLNet_GetError());
        return -1;
    }
    
    // creation of the listening TCP socket
    
    ret = SDLNet_ResolveHost(&ip, NULL, atoi(argv[1]));
    if (ret != 0)
    {
        fprintf(stderr, "setup_server: SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return -1;
    }
    
    listeningtcpsock = SDLNet_TCP_Open(&ip);
    if (listeningtcpsock == NULL)
    {
        fprintf(stderr, "setup_server: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return -1;
    }
    
    // creation of the socket set
    
    set = SDLNet_AllocSocketSet(MAX_CLIENTS+2);
    if (set == NULL)
    {
        fprintf(stderr, "setup_server: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return -1;
    }
    
    ret = SDLNet_TCP_AddSocket(set, listeningtcpsock);
    if (ret == -1)
    {
        fprintf(stderr, "setup_server: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        return -1;
    }
    
    //TODO: add later the UDP listening socket
    
    return 0;
}

int add_client(TCPsocket sock)
{
    int ret = 0;
    
    if (sock == NULL)
    {
        fprintf(stderr, "add_client: Invalid argument(s)\n");
        return -1;
    }
    
    if (num_clients >= MAX_CLIENTS)
    {
        fprintf(stderr, "add_client: Can't add a new client, max count of clients reached.\n");
        return -2;
    }
    
    clients[num_clients] = sock;
    num_clients++;
    
    ret = SDLNet_TCP_AddSocket(set, sock);
    if (ret == -1)
    {
        fprintf(stderr, "add_client: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        return -1;
    }
    
    return 0;
}

int remove_client(int i)
{
    int ret = 0;
    
    ret = SDLNet_TCP_DelSocket(set, clients[i]);
    if (ret == -1)
    {
        fprintf(stderr, "remove_client: SDLNet_TCP_DelSocket: %s\n", SDLNet_GetError());
        return -1;
    }
    
    SDLNet_TCP_Close(clients[i]);
    clients[i] = NULL;
    
    // replace the free place with the last client socket
    
    if (num_clients > 1 && i != num_clients-1)
    {
        clients[i] = clients[num_clients-1];
        clients[num_clients-1] = NULL;
    }
    
    num_clients--;
     
    return 0;
}

//TODO: use macros for commands size and value
int handle_client_msg(char * msg)
{
    int i = 0;
    
    if (msg == NULL)
    {
        fprintf(stderr, "handle_client_msg: Invalid argument(s)\n");
        return -1;
    }
    
    //TODO: check with valgrind if there is no memory leaks in the server nor in the client after that
    if (strlen(msg) == 8 && strncmp(msg, "shutdown", 8) == 0)
    {
        fprintf(stderr, "\"shutdown\" command received.\n");
        return -2;
    }
    
    if (strlen(msg) == 3 && strncmp(msg, "who", 3) == 0)
    {
        fprintf(stderr, "\"who\" command received.\n");
        
        //TODO: LOL
        for(i=0;i<num_clients;i++)
        {
            fprintf(stderr, "Client %d\n", i+1);
        }
    }
    
    return 0;
}

void cleanup_server(void)
{
    int i = 0;
    
    if (set != NULL)
        SDLNet_FreeSocketSet(set);
    if (listeningtcpsock != NULL)
        SDLNet_TCP_Close(listeningtcpsock);
    for (i=0;i<num_clients;i++)
        SDLNet_TCP_Close(clients[i]);
    SDLNet_Quit();
    SDL_Quit();
}

