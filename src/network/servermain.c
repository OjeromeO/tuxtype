#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "network.h"



TCPsocket clients[MAX_CLIENTS];
int num_clients = 0;



int main(int argc, char ** argv)
{
    int i = 0;
    int ret = 0;
    char * msg = NULL;
    IPaddress ip = {0, 0};
    //UDPsocket listeningudpsock = NULL;
    TCPsocket listeningtcpsock = NULL;
    TCPsocket clientsock = NULL;
    SDLNet_SocketSet set = NULL;
    //UDPpacket * out = NULL;
    //UDPpacket * in = NULL;
    
    if (argc != 2)
    {
        printf("%s: Invalid arguments\n", argv[0]);
        printf("    Usage: %s port\n", argv[0]);
        
        return EXIT_FAILURE;
    }
    
    // SDL initializations
    
    ret = SDL_Init(0);
    if (ret != 0)
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    ret = SDLNet_Init();
    if (ret != 0)
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        return EXIT_FAILURE;
    }
    
    // creation of the listening TCP socket
    
    ret = SDLNet_ResolveHost(&ip, NULL, atoi(argv[1]));
    if (ret != 0)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    listeningtcpsock = SDLNet_TCP_Open(&ip);
    if (listeningtcpsock == NULL)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    // creation of the socket set
    
    set = SDLNet_AllocSocketSet(MAX_CLIENTS+2);
    if (set == NULL)
    {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(listeningtcpsock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    ret = SDLNet_TCP_AddSocket(set, listeningtcpsock);
    if (ret == -1)
    {
        fprintf(stderr, "SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        SDLNet_FreeSocketSet(set);
        SDLNet_TCP_Close(listeningtcpsock);
        SDLNet_Quit();
        SDL_Quit();
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
        
        // reset the socket set
        
        if (set != NULL)
            SDLNet_FreeSocketSet(set);
        
        set = SDLNet_AllocSocketSet(num_clients+1);
        if (set == NULL)
        {
            fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
            SDLNet_TCP_Close(listeningtcpsock);
            for (i=0;i<num_clients;i++)
                SDLNet_TCP_Close(clients[i]);
            SDLNet_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        SDLNet_TCP_AddSocket(set, listeningtcpsock);
        for(i=0;i<num_clients;i++)
            SDLNet_TCP_AddSocket(set, clients[i]);
        
        // check sockets activity
        
        ret = SDLNet_CheckSockets(set, -1);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            perror("SDLNet_CheckSockets");
            SDLNet_FreeSocketSet(set);
            SDLNet_TCP_Close(listeningtcpsock);
            for (i=0;i<num_clients;i++)
                SDLNet_TCP_Close(clients[i]);
            SDLNet_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        // accept incoming TCP connections
        
        if (ret > 0 && SDLNet_SocketReady(listeningtcpsock))
        {
            clientsock = SDLNet_TCP_Accept(listeningtcpsock);
            
            if (clientsock != NULL)
            {
                if (num_clients < MAX_CLIENTS)
                {
                    clients[num_clients] = clientsock;
                    num_clients++;
                    fprintf(stderr, "New client !\n");
                }
                else
                {
                    fprintf(stderr, "Can't add a new client, max count of clients reached.\n");
                    SDLNet_TCP_Close(clientsock);
                }
            }
        }
        
        // receive clients messages
        
        for (i=0;i<num_clients;i++)
        {
            if (ret > 0 && SDLNet_SocketReady(clients[i]))
            {
                switch (RecvMessage(clients[i], &msg))
                {
                    case 0:     fprintf(stderr, "=> %s\n", msg);
                                break;
                                
                    case -1:    SDLNet_FreeSocketSet(set);
                                SDLNet_TCP_Close(listeningtcpsock);
                                for (i=0;i<num_clients;i++)
                                    SDLNet_TCP_Close(clients[i]);
                                SDLNet_Quit();
                                SDL_Quit();
                                return EXIT_FAILURE;
                                break;
                                
                    case -2:    //TODO: change the behaviour for that case,
                                //      just delete the client
                                SDLNet_FreeSocketSet(set);
                                SDLNet_TCP_Close(listeningtcpsock);
                                for (i=0;i<num_clients;i++)
                                    SDLNet_TCP_Close(clients[i]);
                                SDLNet_Quit();
                                SDL_Quit();
                                return EXIT_FAILURE;
                                break;
                }
            }
        }
    }
    
    //TODO: fonction cleanup prenant en arg les trucs pojnteurs et tout...
    
    // cleanup    
    
    //SDLNet_FreePacket(in);
    //SDLNet_FreePacket(out);
    
    SDLNet_FreeSocketSet(set);
    
    for (i=0;i<num_clients;i++)
        SDLNet_TCP_Close(clients[i]);
    
    //SDLNet_UDP_Close(listeningudpsock);
    SDLNet_TCP_Close(listeningtcpsock);
    
    SDLNet_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

