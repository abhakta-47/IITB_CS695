#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/printk.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/signal.h>
#include <linux/uaccess.h>

#include "driver2.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arnab Bhakta");
MODULE_DESCRIPTION("Driver2 IOCTL Device Driver");

#define DRIVER_NAME "driver2"
#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int driver2_open(struct inode *inode, struct file *file) { return 0; }

static int driver2_release(struct inode *inode, struct file *file) { return 0; }

static long driver2_ioctl(struct file *file, unsigned int cmd,
                          unsigned long arg) {
    pid_t target_pid;
    struct task_struct *current_task = current;
    struct pid *pid_struct;
    struct task_struct *new_parent_task;

    switch (cmd) {
    case IOCTL_CHANGE_PARENT:
        if (copy_from_user(&target_pid, (pid_t *)arg, sizeof(pid_t)))
            return -EFAULT;

        printk(KERN_DEBUG "[DRIVER2.2] received target_pid %d\n", target_pid);

        printk(KERN_INFO "[DRIVER2.2] Changing parent of process %d to %d\n",
               current_task->pid, target_pid);

        rcu_read_lock();
        pid_struct = find_get_pid(target_pid);
        if (!pid_struct) {
            printk(KERN_ERR "[DRIVER2.2] find_get_pid failed for PID %d\n",
                   target_pid);
            rcu_read_unlock();
            return -ESRCH;
        }

        new_parent_task = pid_task(pid_struct, PIDTYPE_PID);
        if (!new_parent_task) {
            printk(KERN_ERR "[DRIVER2.2] pid_task failed for PID %d\n",
                   target_pid);
            rcu_read_unlock();
            return -ESRCH;
        }
        rcu_read_unlock();

        printk(KERN_INFO "[DRIVER2.2] Parent process found: PID %d\n",
               target_pid);

        if (!current_task) {
            printk(KERN_ERR "[DRIVER2.2] Invalid current task\n");
            return -ESRCH;
        }

        task_lock(current_task);
        list_del_rcu(&current_task->sibling);
        rcu_assign_pointer(current_task->parent, new_parent_task);
        list_add_rcu(&current_task->sibling, &new_parent_task->children);
        task_unlock(current_task);

        printk(KERN_INFO "[DRIVER2.2] Parent changed\n");

        break;

    case IOCTL_TERMINATE_CHILDREN:
        if (copy_from_user(&target_pid, (pid_t *)arg, sizeof(pid_t)))
            return -EFAULT;

        struct task_struct *parent_task;
        struct list_head *list;
        struct task_struct *child_task;

        rcu_read_lock();
        pid_struct = find_get_pid(target_pid);
        parent_task = pid_task(pid_struct, PIDTYPE_PID);
        rcu_read_unlock();

        if (!parent_task) {
            printk(KERN_ERR "[DRIVER2.2] Invalid parent process ID\n");
            return -EINVAL;
        }

        list_for_each(list, &parent_task->children) {
            child_task = list_entry(list, struct task_struct, sibling);
            send_sig(SIGKILL, child_task, 1);
            printk(KERN_INFO "[DRIVER2.2] Terminated child process %d\n",
                   child_task->pid);
        }

        send_sig(SIGKILL, parent_task, 1);
        printk(KERN_INFO "[DRIVER2.2] Terminated parent process %d\n",
               parent_task->pid);

        break;

    default:
        return -ENOTTY;
    }

    return 0;
}

static struct file_operations fops = {.owner = THIS_MODULE,
                                      .open = driver2_open,
                                      .release = driver2_release,
                                      .unlocked_ioctl = driver2_ioctl};

static int __init driver2_init(void) {
    int ret;
    struct device *dev_ret;

    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, DRIVER_NAME)) <
        0) {
        return ret;
    }

    cdev_init(&c_dev, &fops);

    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
        unregister_chrdev_region(dev, MINOR_CNT);
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, DRIVER_NAME))) {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, DRIVER_NAME))) {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }

    printk(KERN_INFO "[DRIVER2.2] Module loaded\n");
    return 0;
}

static void __exit driver2_exit(void) {
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    printk(KERN_INFO "[DRIVER2.2] Module unloaded\n");
}

module_init(driver2_init);
module_exit(driver2_exit);