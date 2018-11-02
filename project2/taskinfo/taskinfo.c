#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>


#define BUFFER_SIZE 128
#define PROC_NAME "pid"
ssize_t proc_read(struct file *file, char __user *usr_buf, 
    size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, const char __user *usr_buf, 
    size_t count, loff_t *pos);

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write
};

long pid;
/*This function is called each time /proc/hello is read */
ssize_t proc_read(struct file *file, char __user *usr_buf, 
    size_t count, loff_t *pos)
{
    int rv = 0;

    char buffer[BUFFER_SIZE];
    static int completed = 0;
    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    /*copies kernel space buffer to user space usr_buf */
    raw_copy_to_user(usr_buf, buffer, rv);
    
    struct task_struct * pcb = pid_task (find_vpid (pid), PIDTYPE_PID);;

    if(pcb != NULL)
    {
        printk(KERN_INFO "command = [%s] pid = [%d] state = [%li]\n", pcb->comm, pcb->pid, pcb->state);
    }
    else{
        printk(KERN_INFO "INVALID_PID! buffer = %s\n", buffer);
    }
    return rv;
}

ssize_t proc_write(struct file * file, const char __user *usr_buf, 
    size_t count, loff_t *pos)
{
    // int rv = 0;
    char *k_mem;

    /* allocate kernel memory */
    k_mem = kmalloc(count, GFP_KERNEL);

    /* copies user space usr_buf to kernel memory */
    copy_from_user(k_mem, usr_buf, count);

    //printk(KERN_INFO "%s\n", k_mem);
    k_mem[count - 1] = '\0';
    printk (KERN_INFO "count=%d\n", (int)count);
	printk (KERN_INFO "%s\n", k_mem);
	kstrtol (k_mem, 10, &pid);
	printk (KERN_INFO "pid=%ld\n", pid);

    /* return kernel memory */
    kfree(k_mem);

    return count;
}


int simple_init(void)
{
    printk(KERN_INFO "process init\n");
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void)
{
    printk (KERN_INFO "Removing Kernel Module\n");
    remove_proc_entry(PROC_NAME, NULL);
}


/* Macros for registering module entry and exit points. */
module_init (simple_init);
module_exit (simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
