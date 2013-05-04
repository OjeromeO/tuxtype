#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "network.h"



SDLNet_SocketSet serverset = NULL;
TCPsocket listeningtcpsock = NULL;
TCPsocket clients[MAX_CLIENTS];
int num_clients = 0;



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

int RecvMessage(TCPsocket sock, char ** buf)
{
    Uint32 len = -1;
    Uint32 buflen = -1;
    int ret = -1;
    
    if (buf == NULL || sock == NULL)
    {
        fprintf(stderr, "RecvMessage: NULL argument(s)\n");
        *buf = NULL;
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
            fprintf(stderr, "RecvMessage: Connexion closed by client.\n");
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
            fprintf(stderr, "RecvMessage: Connexion closed by client.\n");
            free(*buf);
            *buf = NULL;
            return -2;
        }
    }
    
    return 0;
}

