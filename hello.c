#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

static int AddIntegers(int a, int b)
{
	int sum = a + b;
	return sum;
}

static int __init hello_init(void)
{
	int sum = AddIntegers(1,1);
	printk(KERN_ALERT "Hello world!\n");
	printk(KERN_ALERT "This process is named \"%s\" and has the pid %i\n", current->comm, current->pid);
	printk(KERN_ALERT "AddIntegers() returned %d\n", sum);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);

