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
 * @file    client.c
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   Client management module.
 */

#include "network.h"
#include "client.h"


//TODO: make them static if no other file is going to use them
SDLNet_SocketSet set = NULL;
TCPsocket sock = NULL;



static int setup_client(int argc, char ** argv);
static void cleanup_client(void);



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
    int ret = 0;
    char * msg = NULL;
    
    ret = setup_client(argc, argv);
    if (ret != 0)
    {
        fprintf(stderr, "Can't setup the client.\n");
        cleanup_client();
        return EXIT_FAILURE;
    }
    
    fprintf(stderr, "Client ready.\n");
    
    while (1)
    {
        // check sockets activity
        
        ret = SDLNet_CheckSockets(set, 100);
        if (ret == -1)
        {
            fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            perror("SDLNet_CheckSockets");
            cleanup_client();
            return EXIT_FAILURE;
        }
        
        // receive server messages
        
        if (ret > 0 && SDLNet_SocketReady(sock))
        {
            switch (RecvMessage(sock, &msg))
            {
                case 0:     fprintf(stderr, "=> %s\n", msg);
                            free(msg);
                            break;
                            
                case -1:    fprintf(stderr, "Can't receive the server's message.\n");
                            cleanup_client();
                            return EXIT_FAILURE;
                            break;
                            
                case -2:    fprintf(stderr, "Disconnected from the server.\n");
                            cleanup_client();
                            return EXIT_FAILURE;
                            break;
            }
        }
        
        // send to server messages from stdin
        
        int maxlen = 1024;
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
				ret = SendMessage(sock, message);
				if (ret == -1)
				{
				    fprintf(stderr, "Can't send the message to the server.\n");
				    cleanup_client();
                    return EXIT_FAILURE;
				}
			}
		}
/*
//XXX: Tux Maths read stdin in a different way :

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
    
    cleanup_client();
    
    return EXIT_SUCCESS;
}

int setup_client(int argc, char ** argv)
{
    int ret = 0;
    IPaddress ip = {0, 0};
    
    // handle arguments
    
    if (argc != 3)
    {
        fprintf(stderr, "%s: Invalid arguments\n", argv[0]);
        fprintf(stderr, "    Usage: %s host port\n", argv[0]);
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
    
    //TODO: detect servers    
    
    // creation of the connected socket
    
	if (SDLNet_ResolveHost(&ip, argv[1], atoi(argv[2])) == -1)
	{
		fprintf(stderr, "setup_server: SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return -1;
	}
	
    sock = SDLNet_TCP_Open(&ip);
    if (sock == NULL)
    {
        fprintf(stderr, "setup_server: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return -1;
    }
    
    // creation of the socket set
    
    set = SDLNet_AllocSocketSet(1);
    if (set == NULL)
    {
        fprintf(stderr, "setup_server: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return -1;
    }
    
    ret = SDLNet_TCP_AddSocket(set, sock);
    if (ret == -1)
    {
        fprintf(stderr, "setup_server: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
        return -1;
    }
    
    return 0;
}

void cleanup_client(void)
{
    if (set != NULL)
        SDLNet_FreeSocketSet(set);
    if (sock != NULL)
        SDLNet_TCP_Close(sock);
    SDLNet_Quit();
    SDL_Quit();
}

