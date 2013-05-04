#ifndef _NETWORK_H_
#define _NETWORK_H_



#define MAX_SERVERS 50
#define MAX_CLIENTS 4
#define DEFAULT_PORT 1337
#define CLIENT_DATA "TuxMaths client"
#define SERVER_DATA "TuxMaths server"
#define MSG_MAXLEN 1024



int RecvMessage(TCPsocket sock, char ** buf);



#endif

