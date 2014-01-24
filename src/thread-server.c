#include "common.h"
#include "network.h"

#include <pthread.h>

void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    http_print_request_data(&data);

    write(fd, "received your message\n", 25);
}

void * thr_fn(void *arg) {

    int fd = (*(int *)arg);
    printf("Request handled by %lu \n", pthread_self());
    sleep(3);
    process_request(fd);
    Close(fd);
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
    pthread_t ntid;
    while (1) {
        accept_fd = Accept(listen_fd, &addr, &addr_len);
        printf("Received! \n");

        int tmp_fd = accept_fd;
        pthread_create(&ntid, NULL, thr_fn, (void *)(&tmp_fd));
    }

    Close(listen_fd);
    printf("%s shutting down gracefully \n", prog_name);

    return EXIT_SUCCESS;
}

