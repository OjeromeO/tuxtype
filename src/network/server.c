#include "network.h"
#include "server.h"



SDLNet_SocketSet set = NULL;
TCPsocket listeningtcpsock = NULL;
TCPsocket clients[MAX_CLIENTS] = {0};
int num_clients = 0;



static int setup_server(int argc, char ** argv);
static int create_socketset(void);
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
        
        // update the socket set
        
        //TODO: useless to recreate it each time, just do it one time before the
        //      while, then use new functions like addclient() and delclient()
        //      to handle clients[]+socketset
        
        ret = create_socketset();
        if (ret != 0)
        {
            cleanup_server();
            return EXIT_FAILURE;
        }
        
        // check sockets activity
        
        ret = SDLNet_CheckSockets(set, -1);
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
            //TODO: need an addclient(socket, clients[], num_clients, socketset) function
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
                switch (RecvMessage(&clients[i], &msg))
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

int create_socketset(void)
{
    int ret = 0;
    TCPsocket tcpsockets[MAX_CLIENTS+1] = {0};
    UDPsocket udpsockets[1] = {0};
    
    memcpy(tcpsockets, clients, (num_clients)*sizeof(TCPsocket));
    tcpsockets[num_clients] = listeningtcpsock;
    udpsockets[0] = NULL;  //TODO: will be replaced by the UDP listening socket
    
    ret = CreateSocketSet(&set, tcpsockets, num_clients+1, udpsockets, 1);
    if (ret != 0)
    {
        return -1;
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

