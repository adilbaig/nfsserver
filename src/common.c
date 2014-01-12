#include "common.h"

void unix_error(char *msg) /* unix-style error */
{
    /*fprintf(stderr, "%s: %s\n", msg, strerror(errno));*/
    perror(msg);
    exit(0);
}

int Open_listenfd(int port) {

    int rv = open_listenfd(port);
    if (rv < 0)
        unix_error("Could not listen on the given port \n");

    return rv;
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    int rv;
    if ((rv = accept(s, addr, addrlen)) < 0)
        unix_error("Accept error");
    return rv;
}

void Close(int fd)
{
    int rc;
    if ((rc = close(fd)) < 0)
        unix_error("Close error");
}
