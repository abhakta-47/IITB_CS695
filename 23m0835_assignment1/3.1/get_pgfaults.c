#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmstat.h>

#define PROCFS_NAME "get_pgfaults"

static ssize_t get_pgfaults_show(struct file *file_pointer, char __user *buffer,
                                 size_t buffer_length, loff_t *offset) {
    unsigned long total_pagefaults = 0;
    char s[30];
    int len;
    ssize_t ret;

    total_pagefaults = global_node_page_state(NR_ANON_MAPPED) +
                       global_node_page_state(NR_FILE_MAPPED);

    snprintf(s, sizeof(s), "Total Pagefaults: %lu\n", total_pagefaults);
    len = strlen(s);
    ret = len;

    if (*offset >= len || copy_to_user(buffer, s, len)) {
        ret = 0;
    } else {
        *offset += len;
    }

    return ret;
}

static const struct proc_ops get_pgfaults_fops = {
    .proc_read = get_pgfaults_show,
};

static int __init get_pgfaults_init(void) {
    struct proc_dir_entry *entry;

    entry = proc_create(PROCFS_NAME, 0, NULL, &get_pgfaults_fops);
    if (!entry) {
        pr_err("Failed to create /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit get_pgfaults_exit(void) {
    remove_proc_entry(PROCFS_NAME, NULL);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(get_pgfaults_init);
module_exit(get_pgfaults_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Get Page Faults Module");