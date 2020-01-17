#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

static int __init hello_start(void)
{
	printk(KERN_ALERT "Hello world!");
	return 0;
}

static void __exit hello_end(void)
{
	printk(KERN_ALERT "Goodbye");
}

module_init(hello_start);
module_exit(hello_end);

