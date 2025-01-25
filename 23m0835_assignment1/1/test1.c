/************************************************
CS695 Assignment 01 : 1.4 lkm4_test.c
submitted by Arnab Bhakta (23m0835)
*************************************************
Write a test program test1.c, that takes the number of pages to be allocated
and a stride as CLI arguments. The program should first allocate the required
pages and start from the first allocated address, write using that address, and
continue writing at the next address, which is stride bytes away from the
current address till the end of the allocated region. For example, an allocation
of 128MB and a stride of 16MB will result in 8 writes. Observe the mapped memory
stats using your LKM for different combinations of the input tuple. Note that
allocation of memory and accessing memory are two different actions. Write a
short note on your observation in lkm4.txt and draw a graph with allocated pages
on the x-axis, the mapped physical memory size of the process for different
stride values. Make sure that you allocate memory in the granularity of page
size. Name the source file as lkm4.c and lkm4.png for the graph.
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_pages> <stride_in_pages>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int num_pages = atoi(argv[1]);
    int stride_pages = atoi(argv[2]);
    int page_size = getpagesize();
    size_t total_size = num_pages * page_size;
    size_t stride_size = stride_pages * page_size;

    // Print the process ID
    printf("Process ID: %d\n", getpid());

    // Allocate memory
    char *memory = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    // Write to memory at intervals of stride_size
    for (size_t offset = 0; offset < total_size; offset += stride_size) {
        memory[offset] = 'A'; // Write a character to the memory
        printf("Wrote at stride %zu\n", offset);
        getchar(); // Wait for any key press
    }

    // Clean up
    if (munmap(memory, total_size) == -1) {
        perror("munmap");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}