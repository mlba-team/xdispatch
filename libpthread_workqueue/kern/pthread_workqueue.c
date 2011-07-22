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

#define PWQ_DEVICE_NAME "pthread_workqueue"

/*
 * Prototypes
 */
static int pwq_open (struct inode *inode, struct file *file);
static int pwq_release (struct inode *inode, struct file *file);

/*
 * Global variables
 */
static struct task_struct *pwq_collector_thread;
static int pwq_major;
static struct class *pwq_class;
struct file_operations fops = {
    .owner  =   THIS_MODULE,
    .open	=   pwq_open,
    .release =  pwq_release,
    //TODO: .poll = pwq_poll,
};

//only for sleeping during testing
#include <linux/delay.h>
static int pwq_collector_main(void *arg)
{
    printk(KERN_INFO "pwq collector thread started...\n");
    while (!kthread_should_stop()) {
        msleep(5000);
        printk(KERN_INFO "pwq collector thread awake...\n");
    }
    printk(KERN_INFO "pwq collector thread stopping...\n");

    return 0;
}

static int pwq_open (struct inode *inode, struct file *file) 
{
    printk("pwq_open: pid %d opened the device node\n", current->pid);

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

static int pwq_release (struct inode *inode, struct file *file) 
{
    printk("pwq_release: pid %d closed the device node\n", current->pid);
#ifdef DEADWOOD
    kfree(file->private_data);
#endif

    return 0;
}

static int __init pwq_start(void)
{
    int rv = 0;

    printk(KERN_INFO "Loading pthread_workqueue module...\n");

    /* Register as a character device */
    pwq_major = register_chrdev(0, "pwq", &fops);
    if (pwq_major < 0) {
        printk(KERN_WARNING "register_chrdev() failed");
        return pwq_major;
    }

    /* Create a device node */
    pwq_class = class_create(THIS_MODULE, PWQ_DEVICE_NAME);
    device_create(pwq_class, NULL, MKDEV(pwq_major,0), NULL, PWQ_DEVICE_NAME);

    printk(KERN_INFO "Creating helper thread...\n");
    pwq_collector_thread = kthread_create(pwq_collector_main, NULL, "pwq");
    if (IS_ERR(pwq_collector_thread)) {
        rv = PTR_ERR(pwq_collector_thread);
        goto err_out;
    }
    wake_up_process(pwq_collector_thread);

    printk(KERN_INFO "Finished loading pthread_workqueue module...\n");
    return rv;

err_out:
    //TODO: cleanup
    return rv;
}

static void __exit pwq_end(void)
{
    printk(KERN_INFO "Unloading pthread_workqueue module\n");

    /* Remove the device node */
    device_destroy(pwq_class, MKDEV(pwq_major,0));
    class_destroy(pwq_class);
    unregister_chrdev(pwq_major, PWQ_DEVICE_NAME);

    kthread_stop(pwq_collector_thread);
}

module_init(pwq_start);
module_exit(pwq_end);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Mark Heily <mark@heily.com>");
MODULE_DESCRIPTION("pthread_workqueue");
