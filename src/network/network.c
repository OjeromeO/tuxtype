#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "network.h"



SDLNet_SocketSet serverset = NULL;
TCPsocket listeningtcpsock = NULL;
TCPsocket clients[MAX_CLIENTS] = {0};
int num_clients = 0;

SDLNet_SocketSet clientset = NULL;
TCPsocket clientsock = NULL;



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
    
    return 0;
}
/*
int create_socketset(void)
{
    int i = 0;
    int ret = 0;
    
    if (serverset != NULL)
        SDLNet_FreeSocketSet(serverset);
    
    serverset = SDLNet_AllocSocketSet(MAX_CLIENTS+2);
    if (serverset == NULL)
    {
        fprintf(stderr, "create_socketset: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return -1;
    }
    
    ret = SDLNet_TCP_AddSocket(serverset, listeningtcpsock);
    if (ret == -1)
    {
        fprintf(stderr, "create_socketset: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        return -1;
    }
    
    for(i=0;i<num_clients;i++)
    {
        ret = SDLNet_TCP_AddSocket(serverset, clients[i]);
        if (ret == -1)
        {
            fprintf(stderr, "create_socketset: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
            return -1;
        }
    }
    
    return 0;
}
*/
int CreateSocketSet(SDLNet_SocketSet * set, TCPsocket tcpsockets[], int tcpcount, UDPsocket udpsockets[], int udpcount)
{
    int i = 0;
    int ret = 0;
    
    if (set == NULL
     || ((tcpsockets == NULL || tcpcount <= 0) && (udpsockets == NULL || udpcount <= 0)))
    {
        fprintf(stderr, "create_socketset: Bad argument(s)\n");
        return EXIT_FAILURE;
    }
    
    if (*set != NULL)
        SDLNet_FreeSocketSet(*set);
    
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

int RecvMessage(TCPsocket * sock, char ** buf)
{
    Uint32 len = -1;
    Uint32 buflen = -1;
    int ret = -1;
    
    if (buf == NULL || sock == NULL)
    {
        fprintf(stderr, "RecvMessage: Bad argument(s)\n");
        return -1;
    }
    
    ret = SDLNet_TCP_Recv(*sock, &buflen, sizeof(buflen));
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
            fprintf(stderr, "Connexion closed by the other side.\n");
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
    if (*buf == NULL)
    {
        fprintf(stderr, "RecvMessage: malloc: Can't allocate memory for a message\n");
        return -1;
    }
    
    ret = SDLNet_TCP_Recv(*sock, *buf, len);
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
            fprintf(stderr, "Connexion closed by the other side.\n");
            free(*buf);
            *buf = NULL;
            return -2;
        }
    }
    
    return 0;
}

void cleanup_server(void)
{
    int i = 0;
    
    if (serverset != NULL)
        SDLNet_FreeSocketSet(serverset);
    if (listeningtcpsock != NULL)
        SDLNet_TCP_Close(listeningtcpsock);
    for (i=0;i<num_clients;i++)
        SDLNet_TCP_Close(clients[i]);
    SDLNet_Quit();
    SDL_Quit();
}

void cleanup_client(void)
{
    if (clientset != NULL)
        SDLNet_FreeSocketSet(clientset);
    if (clientsock != NULL)
        SDLNet_TCP_Close(clientsock);
    SDLNet_Quit();
    SDL_Quit();
}

