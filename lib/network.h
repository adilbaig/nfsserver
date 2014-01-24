#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

#define LISTENQ  1024

typedef struct sockaddr SA;

int open_listenfd(int port);
int open_clientfd(char *hostname, int port);
