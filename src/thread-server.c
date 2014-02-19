#include "../lib/common.h"
#include "../lib/threadpool.h"
#include "../lib/response.h"

#define NUM_THREADS 2

void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    http_print_request_data(&data);

    char path[1024] = "data/";
    if (strcmp(data.url.path, "/") == 0) {
        strcat(path, "index.html");
    } else {
        strcat(path, data.url.path);
    }

    struct response *res = response_create();
    response_populate(path, res);
    write(fd, (char *)res->content, res->length);
    write(fd, "\n", 1);
    response_destroy(res);
}

void * thr_fn(void *arg) {

    int fd = (*(int *)arg);
    free(arg);
    printf("%lu:  Request handled. FD: %d \n", (unsigned long)pthread_self(), fd);
    process_request(fd);
    write(fd, "Received your message\n", 25);
    Close(fd);
    printf("%lu: Closed FD: %d \n", (unsigned long)pthread_self(), fd);
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
    int *accept_fd;
    thread_pool_init(NUM_THREADS);
    static int count = 0;

    while (1) {
        accept_fd = malloc(sizeof(int));
        *accept_fd = Accept(listen_fd, &addr, &addr_len);
        threadpool_queue_task_push(&thr_fn, (void *)(accept_fd));
        printf("Request COUNT: %d \n", ++count);
    }

    thread_pool_shutdown();
    Close(listen_fd);
    printf("%s shutting down gracefully \n", prog_name);

    return EXIT_SUCCESS;
}

