/************************************************
CS695 Assignment 01 : 1.1/2 lkm2.c
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

static int target_pid = 0; // Default value for the target process ID

module_param(target_pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(target_pid, "Target process ID");

static int __init lkm2_init(void) {
    struct task_struct *task, *child;

    printk(KERN_INFO "Listing child processes and their states for PID: %d\n", target_pid);

    // Find the target process by its PID
    task = pid_task(find_get_pid(target_pid), PIDTYPE_PID);

    if (!task) {
        printk(KERN_INFO "Process with PID %d not found\n", target_pid);
        return -EINVAL;
    }

    // Iterate through each child process
    list_for_each_entry(child, &task->children, sibling) {
        printk(KERN_INFO "Child PID: %d, State: %u\n", child->pid, child->__state);
    }

    return 0;
}

static void __exit lkm2_exit(void) {
    printk(KERN_INFO "Module unloaded\n");
}

module_init(lkm2_init);
module_exit(lkm2_exit);
