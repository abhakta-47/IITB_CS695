/************************************************
CS695 Assignment 01 : 2.1 program.c
submitted by Arnab Bhakta (23m0835)
*************************************************
- User space program to be used with driver.c
*************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "driver.h"

int main(int argc, char *argv[]) {
    int fd;
    AdressMapperData *map_arg;
    WriteMemoryData *write_data;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <block_size>\n", argv[0]);
        return -1;
    }

    size_t count = atoi(argv[1]);
    if (count <= 0 || count > 1024) {
        fprintf(stderr, "Invalid count value\n");
        return -1;
    }
    size_t i;

    fd = open("/dev/driver1", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return -1;
    }

    char *allocated_memory = (char *)malloc(count);
    if (!allocated_memory) {
        perror("Failed to allocate memory");
        close(fd);
        return -1;
    }

    for (i = 0; i < count; i++) {
        allocated_memory[i] = 104 + i;
    }

    map_arg = (AdressMapperData *)malloc(sizeof(AdressMapperData) +
                                         sizeof(unsigned long) * count);
    if (!map_arg) {
        perror("Failed to allocate memory for map_arg");
        free(allocated_memory);
        close(fd);
        return -1;
    }

    for (i = 0; i < count; i++) {
        map_arg->addresses[i] = (unsigned long)&allocated_memory[i];
        printf("Virtual Address: %p, Value: %d\n",
               (void *)map_arg->addresses[i], allocated_memory[i]);
    }

    map_arg->count = count;

    if (ioctl(fd, IOCTL_GET_PHYSICAL_ADDRESS, map_arg) < 0) {
        perror("IOCTL_GET_PHYSICAL_ADDRESS failed");
        free(allocated_memory);
        free(map_arg);
        close(fd);
        return -1;
    }

    for (i = 0; i < count; i++) {
        printf("Virtual Adress: %lx Physical Address: %lx\n",
               &allocated_memory[i], map_arg->addresses[i]);
    }

    write_data = (WriteMemoryData *)malloc(sizeof(WriteMemoryData) +
                                           sizeof(AdrressValue) * count);
    if (!write_data) {
        perror("Failed to allocate memory for write_data");
        free(allocated_memory);
        free(map_arg);
        close(fd);
        return -1;
    }

    for (i = 0; i < count; i++) {
        write_data->address_values[i].address = map_arg->addresses[i];
        write_data->address_values[i].value = 53 + i;
    }
    write_data->count = count;

    if (ioctl(fd, IOCTL_WRITE_MEMORY, write_data) < 0) {
        perror("IOCTL_WRITE_MEMORY failed");
        free(allocated_memory);
        free(map_arg);
        free(write_data);
        close(fd);
        return -1;
    }

    for (i = 0; i < count; i++) {
        printf("Modified Value at Virtual Address %p: %d\n",
               (void *)map_arg->addresses[i], allocated_memory[i]);
    }

    free(allocated_memory);
    free(map_arg);
    free(write_data);
    close(fd);

    return 0;
}