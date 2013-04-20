#include <SDL/SDL.h>
#include <SDL/SDL_net.h>



int main(int argc, char ** argv)
{
    (void) argc;
    (void) argv;
    
    int ret = 0;
    IPaddress ip;
    TCPsocket mysock = NULL;
    char data[32];
    
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
    
    // creation of the connected socket
    
    /*
    TODO: later, with a better server :
    
    fprintf(stderr, "Please enter your name:\n>\n");
    fgets(buffer, NAME_SIZE, stdin);
    p = strchr(buffer, '\n');  //get rid of newline character
    if(p)
        *p = '\0';

    fprintf(stderr, "Type:\n"
            "'game' to start math game;\n"
            "'exit' to end client leaving server running;\n"
            "'quit' to end both client and server\n>\n");
    */
    // TODO: later, use UDP to detect potential servers,
    //       save their IP, then start TCP connexion
    
    ret = SDLNet_ResolveHost(&ip, "localhost", 1337);
    if (ret != 0)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    mysock = SDLNet_TCP_Open(&ip);
    if (mysock == NULL)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    ret = SDLNet_TCP_Recv(mysock, data, 17);
    if (ret <= 0)
    {
        fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(mysock);
        SDLNet_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    printf("message recu: %s\n", data);
    
    SDLNet_TCP_Close(mysock);
    SDLNet_Quit();
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

