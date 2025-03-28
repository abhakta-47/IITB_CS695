#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// Include header or define the IOCTL call interface and devide name
#include "driver2.h"

//**************************************************

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
    if (argc != 3) {
        printf("Usage: %s <parent_pid> hangs?<1|o>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = atoi(argv[1]);
    pid_t child_pid = getpid();
    bool hang = (argv[2][0] == '1');

    // Open ioctl driver
    int fd_driver = open_driver("/dev/driver2");
    if (fd_driver == -1) {
        perror("ERROR: could not open driver");
        exit(EXIT_FAILURE);
    }

    printf("[CHILD:%d] changing Old parent:%d\n", child_pid, getppid());
    if (ioctl(fd_driver, IOCTL_CHANGE_PARENT, &parent_pid) == -1) {
        perror("ERROR: ioctl failed");
        close_driver("/dev/driver2", fd_driver);
        exit(EXIT_FAILURE);
    }

    close_driver("/dev/driver2", fd_driver);
    if (hang) {
        printf("[CHILD:%d] waiting\n", child_pid);
        sleep(100);
        return EXIT_SUCCESS;
    }
    sleep(5);
    printf("[CHILD:%d] exiting\n", child_pid);
    return EXIT_SUCCESS;
}
