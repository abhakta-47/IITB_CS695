/************************************************
CS695 Assignment 01 : 1.2 lkm2.c
submitted by Arnab Bhakta (23m0835)
*************************************************
Write a kernel module that takes process ID as input,
and for each of its child processes,
print their pid and process state. Name the source file as lkm2.c
*************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION("List child processes and their states");

static int pid = 0; // Default value for the target process ID

module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(pid, "Target process ID");

static const char *get_task_state(long state) {
    switch (state) {
    case TASK_RUNNING:
        return "TASK_RUNNING";
    case TASK_INTERRUPTIBLE:
        return "TASK_INTERRUPTIBLE";
    case TASK_UNINTERRUPTIBLE:
        return "TASK_UNINTERRUPTIBLE";
    case __TASK_STOPPED:
        return "TASK_STOPPED";
    case __TASK_TRACED:
        return "TASK_TRACED";
    case EXIT_DEAD:
        return "EXIT_DEAD";
    case EXIT_ZOMBIE:
        return "EXIT_ZOMBIE";
    case TASK_DEAD:
        return "TASK_DEAD";
    case TASK_WAKEKILL:
        return "TASK_WAKEKILL";
    case TASK_WAKING:
        return "TASK_WAKING";
    case TASK_PARKED:
        return "TASK_PARKED";
    case TASK_NOLOAD:
        return "TASK_NOLOAD";
    case TASK_NEW:
        return "TASK_NEW";
    case TASK_RTLOCK_WAIT:
        return "TASK_RTLOCK_WAIT";
    case TASK_FREEZABLE:
        return "TASK_FREEZABLE";
    case TASK_FROZEN:
        return "TASK_FROZEN";
    case TASK_STATE_MAX:
        return "TASK_STATE_MAX";
    default:
        return "UNKNOWN";
    }
}

static int __init lkm2_init(void) {
    struct task_struct *task, *child;

    printk(KERN_INFO "[LKM2] lkm2 loaded\n");

    printk(KERN_INFO
           "[LKM2] Listing child processes and their states for PID: %d\n",
           pid);

    // Find the target process by its PID
    task = pid_task(find_get_pid(pid), PIDTYPE_PID);

    if (!task) {
        printk(KERN_INFO "[LKM2] Process with PID %d not found\n", pid);
        return -EINVAL;
    }

    // Iterate through each child process
    list_for_each_entry(child, &task->children, sibling) {
        printk(KERN_INFO "[LKM2] Child PID: %d, State: %s\n", child->pid,
               get_task_state(child->__state));
    }

    return 0;
}

static void __exit lkm2_exit(void) {
    printk(KERN_INFO "[LKM2] Module LKM2 unloaded\n");
}

module_init(lkm2_init);
module_exit(lkm2_exit);