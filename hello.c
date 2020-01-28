#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

static char *whom = "world";
static int howmany = 1;


module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);


static void printGreeting(void)
{
	int i = 0;
	for(i=0;i<howmany;i++)
	{
		printk(KERN_ALERT "Hello %s\n", whom);
	}
	printk(KERN_ALERT "This process is named \"%s\" and has the pid %i\n", current->comm, current->pid);
}

static int AddIntegers(int a, int b)
{
	int sum = a + b;
	return sum;
}

static int __init hello_init(void)
{
	dev_t dev;
	int sum = AddIntegers(1,1);
	int major = 0, minor = 22, result = 0;
	dev = MKDEV(major, minor);
	result = register_chrdev_region(dev, 2, "hello_scull");
	if(result <0)
	{
		printk(KERN_ALERT "ERROR: can't get major %d\n", major);
		return result;
	} 
	printGreeting();
	printk(KERN_ALERT "AddIntegers() returned %d\n", sum);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);

