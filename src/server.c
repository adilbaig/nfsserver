#include "common.h"
#include "network.h"

void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    http_print_request_data(&data);

    write(fd, "received your message\n", 25);
}

int main(int argc, char **argv) {

    char *prog_name;
    prog_name = argv[0];

    if (argc < 2) {
        fprintf(stderr, "Usage: nfsserver <listening port> \n");
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    int listen_fd = Open_listenfd(port);
    printf("%s listening INTENTLY on %d \n", prog_name, port);

    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    int accept_fd;
    while (1) {
        accept_fd = Accept(listen_fd, &addr, &addr_len);
        printf("Received! \n");
        process_request(accept_fd);
        Close(accept_fd);
    }

    Close(listen_fd);
    printf("%s shutting down gracefully \n", prog_name);

    return EXIT_SUCCESS;
}

