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
int dev_major = 0, dev_minor = 0;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);


static void printGreeting(int major)
{
	int i = 0;
	for(i=0;i<howmany;i++)
	{
		printk(KERN_ALERT "Hello %s\n", whom);
	}
	printk(KERN_ALERT "This process is named \"%s\" and has the pid %i\n MAJOR=%d", current->comm, current->pid, major);
}

static int AddIntegers(int a, int b)
{
	int sum = a + b;
	return sum;
}

static int __init hello_init(void)
{
	dev_t dev = 0;
	int sum = AddIntegers(1,1);
	int result = 0;
	result = alloc_chrdev_region(&dev, dev_minor, 1, "hello_scull");
	dev_major = MAJOR(dev);
	if(result <0)
	{
		printk(KERN_ALERT "ERROR: can't get major %d\n", dev_major);
		return result;
	} 
	printGreeting(dev_major);
	printk(KERN_ALERT "AddIntegers() returned %d\n", sum);
	return 0;
}

static void __exit hello_exit(void)
{
	dev_t dev;
	dev = MKDEV(dev_major, dev_minor);
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Goodbye! Freeing MAJOR %d\n", dev_major);
}

module_init(hello_init);
module_exit(hello_exit);

