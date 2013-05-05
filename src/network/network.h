#ifndef _NETWORK_H_
#define _NETWORK_H_



#define MAX_SERVERS 50
#define MAX_CLIENTS 4
#define DEFAULT_PORT 1337
#define CLIENT_DATA "TuxMaths client"
#define SERVER_DATA "TuxMaths server"
#define MSG_MAXLEN 1024



//TODO: write the doxygen documentation

extern SDLNet_SocketSet serverset;
extern TCPsocket listeningtcpsock;
extern TCPsocket clients[];
extern int num_clients;

extern SDLNet_SocketSet clientset;
extern TCPsocket clientsock;



int setup_server(int argc, char ** argv);
//int create_socketset(void);
int CreateSocketSet(SDLNet_SocketSet * set, TCPsocket tcpsockets[], int tcpcount, UDPsocket udpsockets[], int udpcount);
int RecvMessage(TCPsocket * sock, char ** buf);
void cleanup_server(void);

void cleanup_client(void);



#endif

