#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIZE (512 * 1024 * 1024) // 512 MB

void enable_thp(int enable) {
    int fd = open("/sys/kernel/mm/transparent_hugepage/enabled", O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (enable) {
        write(fd, "always\n", 7);
    } else {
        write(fd, "never\n", 6);
    }
    close(fd);
}

int main() {
    void *addr;
    pid_t pid = getpid();

    printf("PID: %d\n", pid);

    // Allocate anonymous memory
    addr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,
                -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Touch the pages to ensure they are allocated
    memset(addr, 0, SIZE);

    // Enable THP
    enable_thp(1);
    printf("THP enabled\n");
    getchar(); // Wait for some time to allow the kernel module to check

    // Disable THP
    enable_thp(0);
    printf("THP disabled\n");
    getchar(); // Wait for some time to allow the kernel module to check

    // Clean up
    munmap(addr, SIZE);

    return 0;
}