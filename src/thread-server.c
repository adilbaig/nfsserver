#include "../lib/common.h"
#include "../lib/threadpool.h"

#define NUM_THREADS 2

void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    /*http_print_request_data(&data);*/

    write(fd, "received your message\n", 25);
}

void * thr_fn(void *arg) {

    int fd = (*(int *)arg);
    printf("%lu:  Request handled. FD: %d \n", pthread_self(), fd);
    process_request(fd);
    Close(fd);
    printf("%lu: Closed FD: %d \n", pthread_self(), fd);
    return NULL;
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
    thread_pool_init(NUM_THREADS);

    while (1) {
        accept_fd = Accept(listen_fd, &addr, &addr_len);
        int tmp_fd = accept_fd;
        threadpool_queue_task_push(&thr_fn, (void *)(&tmp_fd));
    }

    thread_pool_shutdown();
    Close(listen_fd);
    printf("%s shutting down gracefully \n", prog_name);

    return EXIT_SUCCESS;
}

