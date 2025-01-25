/************************************************
CS695 Assignment 01 : 2.1 driver.c
submitted by Arnab Bhakta (23m0835)
*************************************************
- Provide the physical address for a given virtual address. The virtual to
physical-address translation should be for the current running process.
- Given a list of physical addresses and corresponding byte values, write the
specified byte value to each of the provided physical memory addresses. The
physical address and the value to be written as parameters of the ioctl call.
*************************************************/

#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "driver.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int __init driver1_ioctl_init(void);
static void __exit driver1_ioctl_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl,
};

static int my_open(struct inode *inode, struct file *file) {
    pr_info("[DRIVER_2.1] Device File Opened...!!!\n");
    return 0;
}

static int my_close(struct inode *inode, struct file *file) {
    pr_info("[DRIVER_2.1] Device File Closed...!!!\n");
    return 0;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    AdressMapperData user_mapper_arg;
    AdressMapperData *mapper_arg;
    WriteMemoryData user_write_data;
    WriteMemoryData *write_data;
    struct page *page;
    unsigned long address;
    size_t i;
    int ret;

    switch (cmd) {
    case IOCTL_GET_PHYSICAL_ADDRESS:
        pr_info("[DRIVER_2.1] starting read");

        // First copy to get the count
        if (copy_from_user(&user_mapper_arg, (AdressMapperData *)arg,
                           sizeof(size_t))) {
            pr_err("[DRIVER_2.1] Unable to read argument");
            return -EFAULT;
        }

        // Allocate memory based on the count
        mapper_arg = kmalloc(sizeof(AdressMapperData) +
                                 sizeof(unsigned long) * user_mapper_arg.count,
                             GFP_KERNEL);
        if (!mapper_arg) {
            pr_err("[DRIVER_2.1] Unable to allocate memory for mapper_arg");
            return -ENOMEM;
        }

        // Copy the full structure including addresses
        if (copy_from_user(mapper_arg, (AdressMapperData *)arg,
                           sizeof(AdressMapperData) +
                               sizeof(unsigned long) * user_mapper_arg.count)) {
            pr_err("[DRIVER_2.1] Unable to read argument");
            kfree(mapper_arg);
            return -EFAULT;
        }

        for (i = 0; i < mapper_arg->count; i++) {
            address = mapper_arg->addresses[i];
            ret = get_user_pages((unsigned long)address, 1, FOLL_WRITE, &page,
                                 NULL);
            if (ret < 0) {
                pr_err("[DRIVER_2.1] Unable to get user pages for address: %lx",
                       address);
                kfree(mapper_arg);
                return -EFAULT;
            }
            unsigned long offset = address & ~PAGE_MASK;
            mapper_arg->addresses[i] = page_to_phys(page) + offset;
            pr_info("[DRIVER_2.1] Virtual Address: %lx, Physical Address: %lx",
                    address, mapper_arg->addresses[i]);
            put_page(page);
        }

        if (copy_to_user((AdressMapperData *)arg, mapper_arg,
                         sizeof(AdressMapperData) +
                             sizeof(unsigned long) * mapper_arg->count)) {
            pr_err("[DRIVER_2.1] Unable to write argument");
            kfree(mapper_arg);
            return -EFAULT;
        }

        kfree(mapper_arg);
        break;

    case IOCTL_WRITE_MEMORY:
        pr_info("[DRIVER_2.1] starting write");

        // First copy to get the count
        if (copy_from_user(&user_write_data, (WriteMemoryData *)arg,
                           sizeof(size_t))) {
            pr_err("[DRIVER_2.1] Unable to read argument");
            return -EFAULT;
        }

        write_data = kmalloc(sizeof(WriteMemoryData) +
                                 sizeof(AdrressValue) * user_write_data.count,
                             GFP_KERNEL);
        if (!write_data) {
            pr_err("[DRIVER_2.1] Unable to allocate memory for write_data");
            return -ENOMEM;
        }

        if (copy_from_user(write_data, (WriteMemoryData *)arg,
                           sizeof(WriteMemoryData) +
                               sizeof(AdrressValue) * user_write_data.count)) {
            pr_err("[DRIVER_2.1] Unable to read argument");
            kfree(write_data);
            return -EFAULT;
        }

        for (i = 0; i < write_data->count; i++) {
            pr_info("[DRIVER_2.1] Addr: %lx Value to write %d",
                    write_data->address_values[i].address,
                    write_data->address_values[i].value);
            if (!pfn_valid(
                    __phys_to_pfn(write_data->address_values[i].address))) {
                pr_err("[DRIVER_2.1] Invalid physical address: %lx",
                       write_data->address_values[i].address);
                kfree(write_data);
                return -EFAULT;
            }
            void __iomem *virt_address = ioremap(
                write_data->address_values[i].address, sizeof(unsigned char));
            if (!virt_address) {
                pr_err("[DRIVER_2.1] Unable to map physical address to virtual "
                       "address");
                kfree(write_data);
                return -EFAULT;
            }
            iowrite8(write_data->address_values[i].value, virt_address);
            iounmap(virt_address);
        }

        kfree(write_data);
        break;

    default:
        pr_err("[DRIVER_2.1] Wrong argument passed");
        return -EINVAL;
    }
    return 0;
}

static int __init driver1_ioctl_init(void) {
    int ret;
    struct device *dev_ret;

    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT,
                                   "driver1_ioctl")) < 0) {
        pr_err("[DRIVER_2.1] Cannot allocate major number\n");
        return ret;
    }
    pr_info("[DRIVER_2.1] Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    cdev_init(&c_dev, &fops);

    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
        pr_err("[DRIVER_2.1] Cannot add the device to the system\n");
        goto r_class;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "char"))) {
        pr_err("[DRIVER_2.1] Cannot create the struct class\n");
        goto r_class;
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, DRIVER_NAME))) {
        pr_err("[DRIVER_2.1] Cannot create the Device 1\n");
        goto r_device;
    }
    pr_info("[DRIVER_2.1] Device Driver Insert...Done!!!\n");
    return 0;

r_device:
    class_destroy(cl);
r_class:
    unregister_chrdev_region(dev, MINOR_CNT);
    return -1;
}

static void __exit driver1_ioctl_exit(void) {
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    pr_info("[DRIVER_2.1] Device Driver Remove...Done!!!\n");
}

module_init(driver1_ioctl_init);
module_exit(driver1_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION("driver1 ioctl() Char Driver");
