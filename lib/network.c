#include "network.h"

/**
 * Create a socket and start listening on the given port.
 */
int open_listenfd(int port) {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind */
    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                (const void *)&optval , sizeof(int)) < 0)
        return -1;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((unsigned short)port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (SA*)&server_address, sizeof(server_address)) < 0)
        return -1;

    if (listen(fd, 1024) < 0)
        return -1;

    return fd;
}

/**
 * Creates a client connection.
 */
int open_clientfd(char *hostname, int port) {

    int fd;
    struct hostent *hp;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;

    hp = gethostbyname(hostname);
    if (hp == NULL)
        return -2;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family =  AF_INET;
    server_address.sin_port = htons((unsigned short)port);
    memcpy((char *)&server_address.sin_addr.s_addr, (char *)hp->h_addr_list[0],
            hp->h_length);

    if (connect(fd, (SA *)&server_address, sizeof(server_address)) < 0)
        return -1;

    return fd;
}
