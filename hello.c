#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

static int hello_init(void)
{
	printk(KERN_ALERT "Hello world!\n");
	printk(KERN_ALERT "This process is named \"%s\" and has the pid %i\n", current->comm, current->pid);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);

