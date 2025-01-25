/************************************************
CS695 Assignment 01 : 1.4 lkm4.c
submitted by Arnab Bhakta (23m0835)
*************************************************
For a specified process (via its PID), determine the size of the allocated
virtual address space (sum of all vmas) and the mapped physical address space.
*************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pagewalk.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION(
    "LKM to calculate virtual and physical memory size of a process");

static int pid = -1;
module_param(pid, int, 0);
MODULE_PARM_DESC(pid, "Process ID");

static void calculate_mem_maps(int pid) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    unsigned long total_vsize = 0;
    unsigned long total_psize = 0;
    unsigned long psize;
    pte_t *pte;
    spinlock_t *ptl;
    unsigned long address;

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_ERR "[LKM4] Task not found for PID %d\n", pid);
        return;
    }

    mm = task->mm;
    if (!mm) {
        printk(KERN_ERR "[LKM4] mm_struct not found for PID %d\n", pid);
        return;
    }

    down_read(&mm->mmap_lock);
    VMA_ITERATOR(iter, mm, 0);
    for_each_vma(iter, vma) {
        total_vsize += vma->vm_end - vma->vm_start;
        for (address = vma->vm_start; address < vma->vm_end;
             address += PAGE_SIZE) {
            psize = 0;
            if (follow_pte(mm, address, &pte, &ptl) == 0) {
                if (pte && pte_present(*pte)) {
                    psize = PAGE_SIZE;
                }
                spin_unlock(ptl);
            }
            total_psize += psize;
        }
    }
    up_read(&mm->mmap_lock);

    printk(KERN_INFO "[LKM4] Virtual Memory Size: %lu KiB\n",
           total_vsize / 1024);
    printk(KERN_INFO "[LKM4] Physical Memory Size: %lu KiB\n",
           total_psize / 1024);
}

static int __init lkm4_init(void) {
    printk(KERN_INFO "[LKM4] Module loaded\n");
    calculate_mem_maps(pid);
    return 0;
}

static void __exit lkm4_exit(void) {
    printk(KERN_INFO "[LKM4] Module LKM4 Unloaded\n");
}

module_init(lkm4_init);
module_exit(lkm4_exit);