#include <sys/wait.h>
#include "../lib/common.h"
#include "../lib/network.h"


void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    http_print_request_data(&data);

    write(fd, "received your message\n", 25);
}

int main(int argc, char **argv) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <listening port> <child processes> \n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    int listen_fd = Open_listenfd(port);
    printf("%s listening INTENTLY on %d \n", argv[0], port);

    int processes = atoi(argv[2]);

    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);

    //Now fork from here. Each process will 'accept'.
    // This part needs to be dynamically monitor processes, and re-launch processes if killed.
    int c=0;
    int live_children = 0;
    while(c++ < processes) {
        pid_t child_pid = fork();
        if(child_pid < 0) {
            perror("fork failure!");
            exit(1);
        }

        if(child_pid == 0) {
            while (1) {
                printf("Listening %d ..\n", c);

                int accept_fd = Accept(listen_fd, &addr, &addr_len);
                printf("Received on %d (PID %d)\n", c, getpid());

                process_request(accept_fd);
                close(accept_fd);
            }
        } else {
            //Parent is counting children forked
            live_children++;
        }
    }

    while(live_children) {
        int status = 0;
        pid_t cp = wait(&status);

        printf("Child (PID %d) killed. ", cp);

        if(WIFEXITED(status))
            printf("Normal termination with exit status=%d\n", WEXITSTATUS(status));

        if(WIFSIGNALED(status))
            printf("Killed by signal=%d\n", WTERMSIG(status));

        if(WIFSTOPPED(status))
            printf("Stopped by signal=%d\n", WSTOPSIG(status));

        if(WIFCONTINUED(status))
            printf("Continued\n");

        live_children--;
    }

    Close(listen_fd);
    printf("Bye!\n");

    return EXIT_SUCCESS;
}

