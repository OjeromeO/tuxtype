#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char ** argv)
{
    (void) argc;
    (void) argv;
    
    int ret = 0;
    IPaddress ip;
    TCPsocket listeningsock = NULL;
    TCPsocket clientsock = NULL;
    int serverrun = 1;
    char * data = "This is a test !";
    int datalen = strlen(data)+1;
    
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
    
    // creation of the listening socket
    
    ret = SDLNet_ResolveHost(&ip, NULL, 1337);
    if (ret != 0)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    listeningsock = SDLNet_TCP_Open(&ip);
    if (listeningsock == NULL)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    // communication
    
    while (serverrun)
    {
        clientsock = SDLNet_TCP_Accept(listeningsock);
        if (clientsock == NULL)
        {
            SDL_Delay(3000);
            continue;
        }
        
        ret = SDLNet_TCP_Send(clientsock, data, datalen);
        if (ret < datalen)
        {
            fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
            SDLNet_TCP_Close(clientsock);
            SDLNet_TCP_Close(listeningsock);
            SDLNet_Quit();
            SDL_Quit();
            return EXIT_FAILURE;
        }
        
        serverrun = 0;
    }

    // sockets cleanup
    
    SDLNet_TCP_Close(clientsock);
    SDLNet_TCP_Close(listeningsock);
    
    
    // SDL cleanup
    
    SDLNet_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

