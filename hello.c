#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

static char *whom = "world";
static int howmany = 1;
int dev_major = 0, dev_minor = 0;

struct scull_qset
{
	void **data;
	struct scull_qset *next;
};

struct scull_dev
{
	struct scull_qset *data;
	int quantum;
	int qset;
	unsigned long size;
	struct cdev cdev;
};
struct scull_dev *scull_devices;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static void printGreeting(int major)
{
	int i = 0;
	for(i=0;i<howmany;i++)
	{
		printk(KERN_ALERT "Hello %s\n", whom);
	}
	printk(KERN_ALERT "This process is named \"%s\" and has the pid %i\n MAJOR=%d\n", current->comm, current->pid, major);
}

static int AddIntegers(int a, int b)
{
	int sum = a + b;
	return sum;
}

int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;
	int i;
	for(dptr = dev->data; dptr = next;)
	{
		if(dptr->data)
		{
			for(i = 0; i < qset; i++)
			{
				kfree(dptr->data[i]);
			}
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = 4000;
	dev->qset = 1000;
	dev->data = NULL;
	return 0;
}

int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev;
	printk(KERN_ALERT "The driver has been called with open()\n");
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;
	if((filp->f_flags & O_ACCMODE) == O_WRONLY)
	{
		printk(KERN_ALERT "The driver has been opened in WRITE ONLY mode!\n");
		scull_trim(dev);
	}
	printk(KERN_ALERT "hello_scull was called with .open()!!\n");
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_ALERT "The driver has been called with read()\n");
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	if(*f_pos >= dev->size)
	{
		printk(KERN_ALERT "*fpos >= dev->size\n");
		goto out;
	}

	if(*f_pos + count > dev->size)
		count = dev->size - *f_pos;
	
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;
	
	/*dptr = scull_follow(dev, item);
	if(dptr == NULL ||!dptr->data || !dptr->data[s_pos])
		goto out;*/
	if(count > quantum - q_pos)
		count = quantum - q_pos;
	if(copy_to_user(buf, dptr->data[s_pos] + q_pos, count))
	{
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	out:
		printk(KERN_ALERT "retval = %zu\n",retval);
		return retval;
}

int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations scull_fops =
{
	.owner = THIS_MODULE,
	.open = scull_open,
	.read = scull_read,
	.release = scull_release,
};

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(dev_major, dev_minor + index);
	printk(KERN_ALERT "Attempting to setup cdev\n");
	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)printk(KERN_ALERT "Error %d adding hello_scull%d", err, index);
	else printk(KERN_ALERT "cdev setup complete.\n");
}

static int __init hello_init(void)
{
	dev_t dev = 0;
	int sum = AddIntegers(1,1);
	int result = 0, i=0;
	result = alloc_chrdev_region(&dev, dev_minor, 1, "scull");
	dev_major = MAJOR(dev);
	if(result <0)
	{
		printk(KERN_ALERT "ERROR: can't get major %d\n", dev_major);
		return result;
	} 
	
	scull_devices = kmalloc(1 * sizeof(struct scull_dev), GFP_KERNEL);
	if(!scull_devices)
	{
		//Should do cleanup and stuff
		printk(KERN_ALERT "ERROR: Couldn't allocate memory for device\n");
		return -1;
	}
	memset(scull_devices, 0, 1 * sizeof(struct scull_dev));
	for(i = 0; i < 1; i++)
	{
		scull_devices[i].quantum = 4000;
		scull_devices[i].qset = 1000;
		scull_setup_cdev(&scull_devices[i], i);
	}
	printGreeting(dev_major);
	printk(KERN_ALERT "AddIntegers() returned %d\n", sum);
	return 0;
}

static void __exit hello_exit(void)
{
	dev_t dev;
	dev = MKDEV(dev_major, dev_minor);
	if(scull_devices)
	{
		printk(KERN_ALERT "Freeing allocated memory\n");
		kfree(scull_devices);
	}
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Goodbye! Freeing MAJOR %d\n", dev_major);
}

module_init(hello_init);
module_exit(hello_exit);

