/************************************************
CS695 Assignment 01 : 1.3 lkm3.c
submitted by Arnab Bhakta (23m0835)
*************************************************
Write a kernel module that takes process ID and a virtual address as its input.
Determine if the virtual address is mapped,
and if so, determine its physical address (pseudo-physical address if running in
a VM). Print the pid, virtual address, and corresponding physical address. Name
the source file as lkm3.c
*************************************************/

#include <asm/page.h>
#include <linux/highmem.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ab47");
MODULE_DESCRIPTION(
    "Check if virtual address is mapped and get physical address");

static int pid = 0;
static unsigned long vaddr = 0;

module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param(vaddr, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

MODULE_PARM_DESC(pid, "Target process ID");
MODULE_PARM_DESC(vaddr, "Target virtual address");

static int __init lkm3_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page;
    unsigned long phys_addr;

    printk(KERN_INFO "[LKM3] Checking virtual address mapping for PID: %d, "
                     "Virtual Address: %lu\n",
           pid, vaddr);

    // Find the target process by its PID
    task = pid_task(find_get_pid(pid), PIDTYPE_PID);

    if (!task) {
        printk(KERN_INFO "[LKM3] Process with PID %d not found\n", pid);
        return -EINVAL;
    }

    mm = get_task_mm(task);

    if (!mm) {
        printk(KERN_INFO "[LKM3] Failed to get memory information for PID %d\n",
               pid);
        return -EINVAL;
    }

    // Translate virtual address to physical address
    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        printk(KERN_INFO "[LKM3] VA not mapped: Invalid PGD\n");
        return -EINVAL;
    }

    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        printk(KERN_INFO "[LKM3] VA not mapped: Invalid P4D\n");
        return -EINVAL;
    }

    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        printk(KERN_INFO "[LKM3] VA not mapped: Invalid PUD\n");
        return -EINVAL;
    }

    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        printk(KERN_INFO "[LKM3] VA not mapped: Invalid PMD\n");
        return -EINVAL;
    }

    pte = pte_offset_map(pmd, vaddr);
    if (pte_none(*pte) || !pte_present(*pte)) {
        printk(KERN_INFO "[LKM3] VA not mapped: Invalid PTE\n");
        return -EINVAL;
    }

    page = pte_page(*pte);
    if (!page) {
        printk(KERN_INFO "[LKM3] VA not mapped: Page not found\n");
        return -EINVAL;
    }

    phys_addr = page_to_pfn(page) | (vaddr & ~PAGE_MASK);

    printk(KERN_INFO "[LKM3] PID: %d, Virtual Address: 0x%lx / %lu\n", pid,
           vaddr, vaddr);
    printk(KERN_INFO "[LKM3] Physical Address: 0x%lx / %lu\n", phys_addr,
           phys_addr);

    return 0;
}

static void __exit lkm3_exit(void) {
    printk(KERN_INFO "[LKM3] Module LKM3 Unloaded\n");
}

module_init(lkm3_init);
module_exit(lkm3_exit);