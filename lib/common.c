#include "common.h"
#include "network.h"

void unix_error(char *msg)
{
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

/**
 * Reads line by line from a given descriptor. If a newline existed it's
 * included in the given buffer
 */
size_t getline_from_fd(const int fd, char *t, size_t maxlen) {

    char c, *buf;
    int count = 0;
    buf = t;
    while (read(fd, &c, 1) > 0) {
        if (c == '\0' || c == '\n' || count == maxlen)
            break;
        *buf = c;
        buf++, count++;
    }

    *buf = '\n';
    buf++;
    *buf = '\0';

    return count;
}
