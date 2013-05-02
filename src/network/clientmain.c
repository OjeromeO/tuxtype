#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#define MAX_SERVERS 50
#define MAX_CLIENTS 4
#define DEFAULT_PORT 1337
#define CLIENT_DATA "TuxMaths client"
#define SERVER_DATA "TuxMaths server"



/*IPaddress servers[MAX_SERVERS];
int num_servers = 0;



int DetectServers(void);



int DetectServers(void)
{
    UDPsocket udpsock = NULL;  
    UDPpacket * out = NULL;
    UDPpacket * in = NULL;
    IPaddress bcast_ip;
    //int sent = 0;
    int attempts = 0;
    int i = 0;
    int ret = 0;
    int numsent = 0;
    //Uint32 timer = 0;
    
    for(i = 0; i < MAX_SERVERS; i++)
        servers[i].host = 0;

    udpsock = SDLNet_UDP_Open(0);
    if(udpsock == NULL)
    {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return -1;
    }
    
    out = SDLNet_AllocPacket(strlen(CLIENT_DATA)+1);
    if (out == NULL)
    {
        SDLNet_UDP_Close(udpsock);
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return -1;
    }
    in = SDLNet_AllocPacket(strlen(SERVER_DATA)+1);
    if (in == NULL)
    {
        SDLNet_UDP_Close(udpsock);
        SDLNet_FreePacket(out);
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return -1;
    }

    ret = SDLNet_ResolveHost(&bcast_ip, "255.255.255.255", DEFAULT_PORT);
    if (ret == -1)
    {
        SDLNet_UDP_Close(udpsock);
        SDLNet_FreePacket(out);
        SDLNet_FreePacket(in);
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return -1;
    }
    
    out->address.host = bcast_ip.host;
    out->address.port = bcast_ip.port;
    sprintf((char *)out->data, CLIENT_DATA);
    out->len = strlen(CLIENT_DATA) + 1;
    
    // make 5 attempts, to counteract the packets loss with UDP protocol
    while(attempts < 5)
    {
        numsent = SDLNet_UDP_Send(udpsock, -1, out);
        if (numsent == 0)
        {
            SDLNet_UDP_Close(udpsock);
            SDLNet_FreePacket(out);
            SDLNet_FreePacket(in);
            fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
            return -1;
        }
        
        //
        //if(!sent)
        //{
        //    DEBUGMSG(debug_lan, "broadcast failed - network inaccessible.\nTrying localhost (for testing)\n");
        //    sent = SDLNet_UDP_Send(udpsock, -1, out_local);
        //}
        //
        
        // wait for servers response
        SDL_Delay(50);

        while ((ret = SDLNet_UDP_Recv(udpsock, in)) == 1)
        {
            printf("recu: %s\n", in->data);
            
            if(strncmp((char*)in->data, SERVER_DATA, strlen(SERVER_DATA)) == 0)
            {
                if (num_servers < MAX_SERVERS)
                {
                    int found = 0;
                    
                    for (i=0;i<num_servers;i++)
                    {
                        if (servers[i].host == in->address.host)
                        {
                            found = 1;
                        }
                    }
                    
                    if (found == 0)
                    {
                        servers[num_servers] = in->address;
                        num_servers++;
                    }
                }
                else
                {
                    fprintf(stderr, "Can't add a new server, max count of servers reached.\n");
                }
            }
            else
            {
                fprintf(stderr, "Not a tuxtype packet\n");
            }
        }
        
        if (ret == -1)
        {
            SDLNet_UDP_Close(udpsock);
            SDLNet_FreePacket(out);
            SDLNet_FreePacket(in);
            fprintf(stderr, "SDLNet_UDP_Recv: %s\n", SDLNet_GetError());
            return -1;
        }
        
        attempts++;
    }

    SDLNet_FreePacket(out); 
    SDLNet_FreePacket(in); 
    SDLNet_UDP_Close(udpsock); 
    return num_servers;
}*/



int main(int argc, char ** argv)
{
    int ret = -1;
    char * buf = NULL;
    Uint32 buflen = -1;
    Uint32 tmplen = -1;
    IPaddress ip = {0, 0};
    TCPsocket sock = NULL;
    SDLNet_SocketSet set = NULL;
    
    if (argc != 3)
    {
        printf("%s: Invalid arguments\n", argv[0]);
        printf("    Usage: %s host port\n", argv[0]);
        
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
    
    //TODO: detect servers    
    
    // creation of the connected socket
    
	printf("Connecting to %s:%d...\n", argv[1], atoi(argv[2]));
	if (SDLNet_ResolveHost(&ip, argv[1], atoi(argv[2])) == -1)
	{
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		SDL_Quit();
		return EXIT_FAILURE;
	}
	
    sock = SDLNet_TCP_Open(&ip);
    if (sock == NULL)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    // creation of the socket set
    
    set = SDLNet_AllocSocketSet(1);
    if (set == NULL)
    {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(sock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    ret = SDLNet_TCP_AddSocket(set, sock);
    if (ret == -1)
    {
        fprintf(stderr, "SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        
        SDLNet_FreeSocketSet(set);
        SDLNet_TCP_Close(sock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    while (1)
    {
        // check sockets activity
        
        ret = SDLNet_CheckSockets(set, 100);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            perror("SDLNet_CheckSockets");
            SDLNet_FreeSocketSet(set);
            SDLNet_TCP_Close(sock);
            SDLNet_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        // receive server messages
        
        if (ret > 0 && SDLNet_SocketReady(sock))
        {
            ret = SDLNet_TCP_Recv(sock, &tmplen, sizeof(tmplen));
            if (ret <= 0)
            {
                if (SDLNet_GetError() != NULL)
                {
                    if (strlen(SDLNet_GetError()) != 0)
                        fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
                    else
                        fprintf(stderr, "SDLNet_TCP_Recv: An error occurred.\n");
                    
                    SDLNet_FreeSocketSet(set);
                    SDLNet_TCP_Close(sock);
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }
                else
                {
                    fprintf(stderr, "Connexion closed by server.\n");
                    SDLNet_FreeSocketSet(set);
                    SDLNet_TCP_Close(sock);
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_SUCCESS;
                }
            }
            
            buflen = SDLNet_Read32(&tmplen);
            if (buflen <= 0)
            {
                fprintf(stderr, "Error, zero length message from the server...\n");
                SDLNet_FreeSocketSet(set);
                SDLNet_TCP_Close(sock);
                SDLNet_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }
            
            buf = malloc(buflen * sizeof(char));
            if (buf == NULL)
            {
                fprintf(stderr, "malloc: can't allocate memory for reception buffer.\n");
                SDLNet_FreeSocketSet(set);
                SDLNet_TCP_Close(sock);
                SDLNet_Quit();
                SDL_Quit();
                return EXIT_FAILURE;
            }
            
            ret = SDLNet_TCP_Recv(sock, buf, buflen);
            if (ret <= 0)
            {
                if (SDLNet_GetError() != NULL)
                {
                    if (strlen(SDLNet_GetError()) != 0)
                        fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
                    else
                        fprintf(stderr, "SDLNet_TCP_Recv: An error occurred.\n");
                    
                    free(buf);
                    SDLNet_FreeSocketSet(set);
                    SDLNet_TCP_Close(sock);
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
                }
                else
                {
                    fprintf(stderr, "Connexion closed by server.\n");
                    free(buf);
                    SDLNet_FreeSocketSet(set);
                    SDLNet_TCP_Close(sock);
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_SUCCESS;
                }
            }
            
            fprintf(stderr, "from server: %s\n", buf);
            free(buf);
        }
        
        // send to server messages from stdin
        
        int maxlen = 1024;
        int len = -1;
        char message[maxlen];
        memset(message, '\0', maxlen);
        fd_set fdset;
        struct timeval tv;
        FD_ZERO(&fdset);
		FD_SET(fileno(stdin), &fdset);
		memset(&tv, 0, sizeof(tv));
		int result = -1;
		result=select(fileno(stdin)+1, &fdset, NULL, NULL, &tv);
		if(result==-1)
		{
			perror("select");
			break;
		}

		if(result && FD_ISSET(fileno(stdin),&fdset))
		{
			if(!fgets(message,maxlen,stdin))
			{
			    fprintf(stderr, "error with fgets\n");
			    break;
			}

			while(strlen(message) && strchr("\n\r\t ",message[strlen(message)-1]))
				message[strlen(message)-1]='\0';

			if(strlen(message))
			{
			    len = strlen(message)+1;
			    len = SDLNet_Write32(len, &len);
				
				if (SDLNet_TCP_Send(sock,&len,sizeof(len)) < sizeof(len))
				{
				    fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
				}
				
				printf("Sending: %s\n",message);
				
				if (SDLNet_TCP_Send(sock,message,strlen(message)+1) < strlen(message)+1)
				{
				    fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
                    SDLNet_Quit();
                    SDL_Quit();
                    return EXIT_FAILURE;
				}
			}
		}
/*
//XXX: Tux Maths does things in a different way :

// we must first set stdin to O_NONBLOCK with:
// fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
int read_stdin_nonblock(char* buf, size_t max_length)
{
    #ifdef HAVE_FCNTL
    int bytes_read = 0;
    char* term = NULL;
    buf[0] = '\0';

    bytes_read = fread (buf, 1, max_length, stdin);
    term = strchr(buf, '\n');
    if (term)
        *term = '\0';

    if(bytes_read > 0)
        bytes_read = 1;
    else
        bytes_read = 0;

    return bytes_read;
    #else
    return 0;
    #endif
}
*/
    }
    
    // cleanup
    
    SDLNet_FreeSocketSet(set);
    SDLNet_TCP_Close(sock);
    SDLNet_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

