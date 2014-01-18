#include "common.h"
#include "network.h"

#define MAXLINE 1024

unsigned int read_line(const int fd, char *t, size_t maxlen) {

    char c, *buf;
    int count = 0;
    buf = t;
    while (read(fd, &c, 1) > 0) {
        if (c == '\0' || c == '\n') {
            break;
        }
        *buf = c;
        buf++, count++;
    }
    *buf = '\n';
    buf++;
    *buf = '\0';
    return count;
}

struct http_header {
    char method[MAXLINE];
    char uri[MAXLINE];
    char version[MAXLINE];
    char user_agent[MAXLINE];
    char host[MAXLINE];
    char accept[MAXLINE];
};

#define PRINT_HEADER_ITEM(item) printf(#item ": %s \n", data->item);
void print_header_data(struct http_header *data) {

    PRINT_HEADER_ITEM(method)
    PRINT_HEADER_ITEM(uri)
    PRINT_HEADER_ITEM(version)
    PRINT_HEADER_ITEM(user_agent)
    PRINT_HEADER_ITEM(host)
    PRINT_HEADER_ITEM(accept)
}

void read_data(int fd) {

    char buf[MAXLINE];
    struct http_header header_data;

    if (read_line(fd, buf, MAXLINE)) {
        sscanf(buf, "%s %s %s", header_data.method, header_data.uri,
                header_data.version);
    } else {
        unix_error("No HTTP header data");
    }

    while(read_line(fd, buf, MAXLINE)) {
        if (strcmp("\r\n", buf) == 0)
            break;

        log_info(buf);

        if (strncmp("User-Agent:", buf, 11) == 0) {
            sscanf(buf, "User-Agent: %[^\n\r]", header_data.user_agent);
        }

        if (strncmp("Host:", buf, 5) == 0) {
            sscanf(buf, "Host: %[^\n\r]", header_data.host);
        }

        if (strncmp("Accept:", buf, 7) == 0) {
            sscanf(buf, "Accept: %[^\n\r]", header_data.accept);
        }
    }

    print_header_data(&header_data);
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
        read_data(accept_fd);
        Close(accept_fd);
    }

    Close(listen_fd);
    printf("%s shutting down gracefully \n", prog_name);

    return EXIT_SUCCESS;
}

