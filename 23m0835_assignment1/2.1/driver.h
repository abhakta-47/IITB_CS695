#ifndef DRIVER_H
#define DRIVER_H

#include <linux/ioctl.h>

#define MY_IOCTL_MAGIC 'k'
#define DRIVER_NAME "driver1"
#define IOCTL_GET_PHYSICAL_ADDRESS _IOWR(MY_IOCTL_MAGIC, 1, AdressMapperData)
#define IOCTL_WRITE_MEMORY _IOW(MY_IOCTL_MAGIC, 2, WriteMemoryData)

typedef struct {
    size_t count;
    unsigned long addresses[];
} AdressMapperData;

typedef struct {
    unsigned long address;
    unsigned char value;
} AdrressValue;

typedef struct {
    size_t count;
    AdrressValue address_values[];
} WriteMemoryData;

#endif /* DRIVER_H */