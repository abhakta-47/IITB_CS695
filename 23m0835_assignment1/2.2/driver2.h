#ifndef DRIVER2_H
#define DRIVER2_H

#include <linux/ioctl.h>

#define MY_IOCTL_MAGIC 'l'
#define IOCTL_CHANGE_PARENT _IOW(MY_IOCTL_MAGIC, 1, pid_t)
#define IOCTL_TERMINATE_CHILDREN _IOW(MY_IOCTL_MAGIC, 2, pid_t)

#define FIRST_MINOR 0
#define MINOR_CNT 1

#endif /* DRIVER2_H */