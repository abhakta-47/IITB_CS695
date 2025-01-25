/************************************************
CS695 Assignment 01 : 1.1 lkm1.c
submitted by Arnab Bhakta (23m0835)
*************************************************
Write a kernel module to list all processes in a running or runnable state.
Print their pid. Name the source file as lkm1.c
*************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION("List processes in a running or runnable state");

static int __init lkm_init(void) {
    struct task_struct *task;

    printk(KERN_INFO "[LKM1] lkm1 loaded\n");
    printk(KERN_INFO
           "[LKM1] Listing processes in a running or runnable state:\n");

    for_each_process(task) {
        if (task->__state == TASK_RUNNING ||
            task->__state == TASK_INTERRUPTIBLE ||
            task->__state == TASK_UNINTERRUPTIBLE) {
            printk(KERN_INFO "[LKM1] PID: %d\n", task->pid);
        }
    }

    return 0;
}

static void __exit lkm_exit(void) {
    printk(KERN_INFO "[LKM1] lkm1 Module unloaded\n");
}

module_init(lkm_init);
module_exit(lkm_exit);
