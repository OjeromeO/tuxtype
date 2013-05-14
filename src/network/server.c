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
#include "messages.h"
#include <math.h>



//TODO: make them static if no other file is going to use them
SDLNet_SocketSet set = NULL;
TCPsocket listeningtcpsock = NULL;
Client clients[MAX_CLIENTS];
int num_clients = 0;
char client_default_name[] = CLIENT_DEFAULT_NAME;

static int setup_server(int argc, char ** argv);
static int add_client(TCPsocket tmpsock);
static int remove_client(int i);
static int handle_client_msg(int client, char * msg);
static void cleanup_server(void);



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
        fprintf(stderr, "Can't setup the server.\n");
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
    
    fprintf(stderr, "Server ready.\n");
    
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
                    case 0:     fprintf(stderr, "========> %s connected\n", (clients[num_clients-1].name == NULL)?client_default_name:clients[num_clients-1].name);
                                break;
                                
                    case -1:    fprintf(stderr, "Can't add a new client.\n");
                                SDLNet_TCP_Close(tmpsock);
                                cleanup_server();
                                return EXIT_FAILURE;
                                break;
                                
                    case -2:    fprintf(stderr, "Can't add a new client.\n");
                                SDLNet_TCP_Close(tmpsock);
                                break;
                }
            }
            
            // receive clients messages
            
            for (i=0;i<num_clients;i++)
            {
                if (SDLNet_SocketReady(clients[i].sock))
                {
                    switch (RecvMessage(clients[i].sock, &msg))
                    {
                        case 0:     ret = handle_client_msg(i, msg);
                                    if (ret == -1)
                                    {
                                        fprintf(stderr, "Can't handle the client's message.\n");
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
                                    
                        case -1:    fprintf(stderr, "Can't receive the client's message.\n");
                                    cleanup_server();
                                    return EXIT_FAILURE;
                                    break;
                                    
                        case -2:    fprintf(stderr, "========> %s disconnected from the server\n", (clients[i].name == NULL)?client_default_name:clients[i].name);
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
    
    //TODO: add the UDP listening socket
    
    return 0;
}

int add_client(TCPsocket sock)
{
    int ret = 0;
    
    if (sock == NULL)
    {
        fprintf(stderr, "add_client: Invalid argument(s).\n");
        return -1;
    }
    
    if (num_clients >= MAX_CLIENTS)
    {
        fprintf(stderr, "add_client: Max count of clients reached.\n");
        return -2;
    }
    
    // initialize the client
    
    clients[num_clients].name = NULL;
    clients[num_clients].sock = sock;
    
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
    
    if (i < 0 || i >= num_clients)
    {
        fprintf(stderr, "remove_client: Invalid argument(s).\n");
        return -1;
    }
    
    if (clients[i].sock != NULL)
    {
        ret = SDLNet_TCP_DelSocket(set, clients[i].sock);
        if (ret == -1)
        {
            fprintf(stderr, "remove_client: SDLNet_TCP_DelSocket: %s\n", SDLNet_GetError());
            return -1;
        }
        SDLNet_TCP_Close(clients[i].sock);
        clients[i].sock = NULL;
    }
    if (clients[i].name != NULL)
    {
        free(clients[i].name);
    }
    
    // replace the free place with the last client
    
    if (num_clients > 1 && i != num_clients-1)
    {
        clients[i].name = clients[num_clients-1].name;
        clients[i].sock = clients[num_clients-1].sock;
        clients[num_clients-1].sock = NULL;
        clients[num_clients-1].name = NULL;
    }
    
    num_clients--;
     
    return 0;
}

int handle_client_msg(int client, char * msg)
{
    int i = 0;
    int ret = 0;
    int countlength = 0;
    int listlen = 0;
    int next = 0;
    char * buf = NULL;
    
    if (client < 0 || client >= num_clients || msg == NULL)
    {
        fprintf(stderr, "handle_client_msg: Invalid argument(s).\n");
        return -1;
    }
    
    if (strlen(msg) == CMD_SHUTDOWN_SIZE
     && strncmp(msg, CMD_SHUTDOWN, CMD_SHUTDOWN_SIZE) == 0)
    {
        fprintf(stderr, "  %s command received.\n", CMD_SHUTDOWN);
        return -2;
    }
    
    if (strlen(msg) == CMD_QUIT_SIZE
     && strncmp(msg, CMD_QUIT, CMD_QUIT_SIZE) == 0)
    {
        fprintf(stderr, "  %s command received.\n", CMD_QUIT);
        ret = remove_client(client);
        if (ret == -1)
        {
            fprintf(stderr, "handle_client_msg: Can't remove the client.\n");
            return -1;
        }
        return 0;
    }
    
    if (strlen(msg) == CMD_COUNT_SIZE
     && strncmp(msg, CMD_COUNT, CMD_COUNT_SIZE) == 0)
    {
        fprintf(stderr, "  %s command received.\n", CMD_COUNT);
        countlength = floor(log10(num_clients))+1;
        buf = malloc((CMD_COUNT_SIZE+1+countlength+1) * sizeof(char));
        memset(buf, '\0', CMD_COUNT_SIZE+1+countlength+1);
        if (buf == NULL)
        {
            fprintf(stderr, "handle_client_msg: malloc: Can't allocate memory for the COUNT response.\n");
            return -1;
        }
        ret = snprintf(buf, CMD_COUNT_SIZE+1+countlength+1, "%s\n%d", CMD_COUNT, num_clients);
        if (ret != (int)CMD_COUNT_SIZE+1+countlength)
        {
            fprintf(stderr, "handle_client_msg: snprintf: Can't write the COUNT response.\n");
            return -1;
        }
        ret = SendMessage(clients[client].sock, buf);
        if (ret != 0)
        {
            fprintf(stderr, "handle_client_msg: Can't send the COUNT response to the client.\n");
            return -1;
        }
        return 0;
    }
    
    if (strlen(msg) == CMD_WHO_SIZE
     && strncmp(msg, CMD_WHO, CMD_WHO_SIZE) == 0)
    {
        listlen += CMD_WHO_SIZE;
        for(i=0;i<num_clients;i++)
        {
            if (clients[i].name != NULL)
            {
                listlen += 1+strlen(clients[i].name);
            }
            else
            {
                listlen += 1+CLIENT_DEFAULT_NAME_SIZE;
            }
        }
        buf = malloc((listlen+1) * sizeof(char));
        memset(buf, '\0', listlen+1);
        memcpy(buf, CMD_WHO, strlen(CMD_WHO));
        next += CMD_WHO_SIZE;
        for(i=0;i<num_clients;i++)
        {
            memcpy(&buf[next], "\n", strlen("\n"));
            next += 1;
            memcpy(&buf[next],
                   (clients[i].name == NULL)?client_default_name:clients[i].name,
                   strlen((clients[i].name == NULL)?client_default_name:clients[i].name));
            next += strlen((clients[i].name == NULL)?client_default_name:clients[i].name);
        }
        fprintf(stderr, "  %s command received.\n", CMD_WHO);
        ret = SendMessage(clients[client].sock, buf);
        if (ret != 0)
        {
            fprintf(stderr, "handle_client_msg: Can't send the WHO response to the client.\n");
            return -1;
        }
        return 0;
    }
    
    if (strlen(msg) >= CMD_NICKNAME_SIZE+2
     && strncmp(msg, CMD_NICKNAME, CMD_NICKNAME_SIZE) == 0)
    {
        fprintf(stderr, "  %s command received.\n", CMD_NICKNAME);
        buf = malloc((strlen(msg)+1-CMD_NICKNAME_SIZE-1) * sizeof(char));
        memset(buf, '\0', strlen(msg)+1-CMD_NICKNAME_SIZE-1);
        if (buf == NULL)
        {
            fprintf(stderr, "handle_client_msg: malloc: Can't allocate memory for the nickname.\n");
            return -1;
        }
        memcpy(buf, &msg[CMD_NICKNAME_SIZE+1], strlen(msg)+1-CMD_NICKNAME_SIZE-1);
        if (clients[client].name != NULL)
        {
            free(clients[client].name);
        }
        clients[client].name = buf;
        return 0;
    }
    
    fprintf(stderr, "%s: %s\n", (clients[client].name == NULL)?client_default_name:clients[client].name, msg);
    
    return 0;
}

void cleanup_server(void)
{
    int i = 0;
    
    if (set != NULL)
    {
        SDLNet_FreeSocketSet(set);
    }
    if (listeningtcpsock != NULL)
    {
        SDLNet_TCP_Close(listeningtcpsock);
    }
    for (i=0;i<num_clients;i++)
    {
        if (clients[i].sock != NULL)
        {
            SDLNet_TCP_Close(clients[i].sock);
        }
        if (clients[i].name != NULL)
        {
            free(clients[i].name);
        }
    }
    SDLNet_Quit();
    SDL_Quit();
}

