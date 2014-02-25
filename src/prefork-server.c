#include <sys/wait.h>
#include "../lib/common.h"
#include "../lib/network.h"


void process_request(int fd) {

    struct http_request_data data = http_parse_request(fd);
    http_print_request_data(&data);

    write(fd, "received your message\n", 25);
}

pid_t fork_child(int listen_fd)
{
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);

    pid_t child_pid = fork();
    if(child_pid < 0) {
        perror("fork failure!");
        exit(1);
    }

    if(child_pid == 0) {

        printf("Child started (PID %d)\n", getpid());

        while (1) {

            int accept_fd = Accept(listen_fd, &addr, &addr_len);
            printf("Received on (PID %d)\n", getpid());

            process_request(accept_fd);
            close(accept_fd);
        }
    } else {
        return child_pid;
    }
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

    //Now fork from here. Each process will 'accept'.
    int c = 0;
    int live_children = 0;
    while(c++ < processes) {
        pid_t child_pid = fork_child(listen_fd);
        if(child_pid != 0) {
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

        //Dynamically check if a child was manually killed via SIGABRT. Restart if so.
        if(WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            printf("Killed by signal=%d.\n", sig);

            //Restart the child process if killed w SIGABRT
            if(sig == SIGABRT) {
                printf("Child was aborted. Restarting process.. ");

                pid_t child_pid = fork_child(listen_fd);
                if(child_pid != 0) {
                    live_children++;
                }
            }
        }

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

