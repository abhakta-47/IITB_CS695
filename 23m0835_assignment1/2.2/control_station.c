#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "driver2.h"

void sigchld_handler(int sig) {
    int status;
    pid_t soldier_pid, last_pid;
    while ((soldier_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (last_pid == soldier_pid)
            break;
        last_pid = soldier_pid;
        if (WIFEXITED(status)) {
            printf(
                "[Parent]: Soldier process %d terminated with exit status %d\n",
                soldier_pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent]: Soldier process %d killed by signal %d\n",
                   soldier_pid, WTERMSIG(status));
        }
        fflush(stdout);
    }
    if (soldier_pid == -1 && errno != ECHILD) {
        perror("waitpid");
    }
}

void sigterm_handler(int sig) {
    printf("[Parent]: Control station %d exiting\n", getpid());
    fflush(stdout);

    exit(0);
}

int open_driver(const char *driver_name) {

    int fd_driver = open(driver_name, O_RDWR);
    if (fd_driver == -1) {
        perror("ERROR: could not open driver");
    }

    return fd_driver;
}

void close_driver(const char *driver_name, int fd_driver) {

    int result = close(fd_driver);
    if (result == -1) {
        perror("ERROR: could not close driver");
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <sleep>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int s_d = atoi(argv[1]);

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    struct sigaction sa_term;
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = SA_RESTART;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction (SIGTERM)");
        exit(1);
    }

    printf("[PARENT:%u] Control station process started\n", getpid());
    sleep(s_d); // Wait for all child process until Emergency is initiated.

    printf("[PARENT]: Emergency Emergency!\n");

    // open ioctl driver
    int fd_driver = open_driver("/dev/driver2");
    if (fd_driver == -1) {
        perror("ERROR: could not open driver");
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = getpid();
    if (ioctl(fd_driver, IOCTL_TERMINATE_CHILDREN, &parent_pid) == -1) {
        perror("ERROR: ioctl failed");
        close_driver("/dev/driver2", fd_driver);
        exit(EXIT_FAILURE);
    }

    close_driver("/dev/driver2", fd_driver);

    printf("[PARENT]: Control station %u exiting", getpid());

    return 0;
}