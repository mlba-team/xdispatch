/*-
 * Copyright (c) 2011 Mark Heily <mark@heily.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/rbtree.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

/*
 * Prototypes
 */
static int tpool_open (struct inode *inode, struct file *file);
static int tpool_release (struct inode *inode, struct file *file);

/*
 * Global variables
 */
static struct task_struct *tpool_manager_thread;
static int tpool_major;
static struct class *tpool_class;
struct file_operations fops = {
    .owner  =   THIS_MODULE,
    .open	=   tpool_open,
    .release =  tpool_release,
    //TODO: .poll = tpool_poll,
};

//only for sleeping during testing
#include <linux/delay.h>
static int tpool_main(void *arg)
{
    printk(KERN_INFO "threadpool thread started...\n");
    while (!kthread_should_stop()) {
        msleep(5000);
        printk(KERN_INFO "threadpool thread awake...\n");
    }
    printk(KERN_INFO "threadpool stopping...\n");

    return 0;
}

static int tpool_open (struct inode *inode, struct file *file) 
{
    printk("tpool_open\n");

#if 0
    kq = kmalloc(sizeof(*kq), GFP_KERNEL);
    if (kq == NULL) {
        printk("kqueue: kmalloc failed\n");
        return -1;
    }
    spin_lock_init(&kq->kq_lock);
    for (i = 0; i < EVFILT_SYSCOUNT; i++) 
        kq->kq_filt[i].kf_note = RB_ROOT;
    file->private_data = kq;
#endif

    return 0;
}

static int tpool_release (struct inode *inode, struct file *file) 
{
    printk("tpool_release\n");
#if DEADWOOD
    kfree(file->private_data);
#endif

    return 0;
}

static int __init tpool_start(void)
{
    int rv = 0;

    printk(KERN_INFO "Loading threadpool module...\n");

    /* Register as a character device */
    tpool_major = register_chrdev(0, "threadpool", &fops);
    if (tpool_major < 0) {
        printk(KERN_WARNING "register_chrdev() failed");
        return tpool_major;
    }

    /* Create /dev/threadpool */
    tpool_class = class_create(THIS_MODULE, "threadpool");
    device_create(tpool_class, NULL, MKDEV(tpool_major,0), NULL, "threadpool");

    printk(KERN_INFO "Creating helper thread...\n");
    tpool_manager_thread = kthread_create(tpool_main, NULL, "threadpool");
    if (IS_ERR(tpool_manager_thread)) {
        rv = PTR_ERR(tpool_manager_thread);
        goto err_out;
    }
    wake_up_process(tpool_manager_thread);

    printk(KERN_INFO "Finished loading threadpool module...\n");
    return rv;

err_out:
    //TODO: cleanup
    return rv;
}

static void __exit tpool_end(void)
{
    printk(KERN_INFO "Unloading threadpool module\n");

    /* Remove /dev/threadpool */
    device_destroy(tpool_class, MKDEV(tpool_major,0));
    class_destroy(tpool_class);
    unregister_chrdev(tpool_major, "threadpool");

    kthread_stop(tpool_manager_thread);
}

module_init(tpool_start);
module_exit(tpool_end);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Mark Heily <mark@heily.com>");
MODULE_DESCRIPTION("thread pool global load balancing");
