/*  
 *  moudule.c - Demonstrates module documentation.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>

#define DRIVER_AUTHOR "Peter Jay Salzman <p@dirac.org>"
#define DRIVER_DESC   "A sample driver"

static int glob_conut = 0;

spinlock_t lock;
DEFINE_MUTEX(mux);

void plus(void)
{
    int tmp;
    // spin_lock(&lock);
    mutex_lock(&mux);
    tmp = glob_conut;
    tmp ++;
    glob_conut = tmp;
    mutex_unlock(&mux);
    // spin_unlock(&lock);
}


/*
 * The mthread thread - touches the timestamp.
 */
static int mthread1(void *unused)
{
    static int count = 60000000;

    static int cpu[4] = {0,0,0,0};

    set_current_state(TASK_INTERRUPTIBLE);

    while (!kthread_should_stop()) {
        if (count-- == 0)
            break;
        plus();

        // cpu[smp_processor_id()] ++;
    }

    __set_current_state(TASK_RUNNING);

    printk(KERN_INFO "-- p1 %d %d\n", count, glob_conut);
    printk(KERN_INFO "-- cpu %d %d %d %d\n", cpu[0], cpu[1],cpu[2],cpu[3]);

    return 0;
}

static int mthread2(void *unused)
{
    static int count = 60000000;

    static int cpu[4] = {0,0,0,0};

    set_current_state(TASK_INTERRUPTIBLE);

    while (!kthread_should_stop()) {
        if (count-- == 0)
            break;
        plus();
        // cpu[smp_processor_id()] ++;
    }

    __set_current_state(TASK_RUNNING);

    printk(KERN_INFO "-- p2 %d %d\n", count, glob_conut);
    printk(KERN_INFO "-- cpu %d %d %d %d\n", cpu[0], cpu[1],cpu[2],cpu[3]);

    return 0;               
}

static int mthread3(void *unused)
{
    static int count = 60000000;

    static int cpu[4] = {0,0,0,0};

    set_current_state(TASK_INTERRUPTIBLE);

    while (!kthread_should_stop()) {
        if (count-- == 0)
            break;
        plus();
        // cpu[smp_processor_id()] ++;
    }

    __set_current_state(TASK_RUNNING);

    printk(KERN_INFO "-- p3 %d %d\n", count, glob_conut);
    printk(KERN_INFO "-- cpu %d %d %d %d\n", cpu[0], cpu[1],cpu[2],cpu[3]);

    return 0;
}

static struct task_struct *p1;
static struct task_struct *p2;
static struct task_struct *p3;

static int __init init_xmodule(void)
{

    spin_lock_init(&lock);
    mutex_init(&mux);

    p1 = kthread_create(mthread1, NULL, "mthread1");
    if (IS_ERR(p1)) {
        printk(KERN_INFO "Cannot create kthread1\n");
        return 0;
    }

    p2 = kthread_create(mthread2, NULL, "mthread2");
    if (IS_ERR(p2)) {
        printk(KERN_INFO "Cannot create kthread2\n");
        return 0;
    }

    p3 = kthread_create(mthread3, NULL, "mthread3");
    if (IS_ERR(p3)) {
        printk(KERN_INFO "Cannot create kthread3\n");
        return 0;
    }

    wake_up_process(p1);
    wake_up_process(p2);
    // wake_up_process(p3);

    printk(KERN_INFO "Hello, world\n");

	return 0;
}

static void __exit cleanup_xmodule(void)
{
    if (p1->state <= 0)
        kthread_stop(p1); // Synchronous operation
    if (p2->state <= 0)
        kthread_stop(p2);
    if (p3->state <= 0)
        kthread_stop(p3);

	printk(KERN_INFO "Goodbye, world\n");
}

module_init(init_xmodule);
module_exit(cleanup_xmodule);
/* 
 * Get rid of taint message by declaring code as GPL. 
 */
MODULE_LICENSE("GPL");

/*
 * Or with defines, like this:
 */
MODULE_AUTHOR(DRIVER_AUTHOR);	/* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);	/* What does this module do */

