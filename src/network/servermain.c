#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "network.h"



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
        
        // reset the socket set
        //TODO: is it really needed to recreate it each time ? check that later
        //      when client deco will be supported (maybe it's not like select()
        //      who modify the sets in arguments)
        
        ret = create_socketset();
        if (ret != 0)
        {
            cleanup_server();
            return EXIT_FAILURE;
        }
        
        // check sockets activity
        
        ret = SDLNet_CheckSockets(serverset, -1);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            perror("SDLNet_CheckSockets");
            cleanup_server();
            return EXIT_FAILURE;
        }
        
        // accept incoming TCP connections
        
        if (ret > 0 && SDLNet_SocketReady(listeningtcpsock))
        {
            tmpsock = SDLNet_TCP_Accept(listeningtcpsock);
            if (tmpsock != NULL)
            {
                if (num_clients < MAX_CLIENTS)
                {
                    clients[num_clients] = tmpsock;
                    num_clients++;
                    fprintf(stderr, "New client !\n");
                }
                else
                {
                    fprintf(stderr, "Can't add a new client, max count of clients reached.\n");
                    SDLNet_TCP_Close(tmpsock);
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
                                
                    case -1:    cleanup_server();
                                return EXIT_FAILURE;
                                break;
                                
                    case -2:    //TODO: change the behaviour for that case,
                                //      just delete the client and DelSocket()
                                cleanup_server();
                                return EXIT_FAILURE;
                                break;
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

