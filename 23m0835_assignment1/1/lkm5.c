/************************************************
CS695 Assignment 01 : 1.5 lkm5.c
submitted by Arnab Bhakta (23m0835)
*************************************************
For the specified process (via its PID) [input should be named pid], determine
the number of huge pages and the allocated virtual address space they use when
THP is enabled and when it is disabled. The source file should be named lkm5.c
*************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION("LKM to check THP usage for a specified process");

static int pid = -1;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "The PID of the process to check");

static int __init lkm5_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long thp_size = 0;
    int thp_count = 0;
    struct vm_area_struct *vma;

    if (pid == -1) {
        printk(KERN_ERR "[LKM5] No PID specified\n");
        return -EINVAL;
    }

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_ERR "[LKM5] Invalid PID\n");
        return -ESRCH;
    }

    mm = task->mm;
    if (!mm) {
        printk(KERN_ERR "[LKM5] No memory management structure\n");
        return -EINVAL;
    }

    down_read(&mm->mmap_lock);
    VMA_ITERATOR(iter, mm, 0);
    for_each_vma(iter, vma) {
        if (vma->vm_flags & VM_HUGETLB) {
            thp_size += vma->vm_end - vma->vm_start;
            thp_count++;
        }
    }
    up_read(&mm->mmap_lock);

    printk(KERN_INFO "[LKM5] THP Size: %lu KiB, THP count: %d\n",
           thp_size >> 10, thp_count);
    return 0;
}

static void __exit lkm5_exit(void) {
    printk(KERN_INFO "[LKM5] Module LKM5 Unloaded\n");
}

module_init(lkm5_init);
module_exit(lkm5_exit);