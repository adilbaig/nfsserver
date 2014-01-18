#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "network.h"

// Error handling prototypes
void unix_error(char *msg);

int Open_listenfd(int port);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
void Close(int fd);

void log_info(char *msg);
