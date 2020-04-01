#include "hello.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANTON");
MODULE_DESCRIPTION("HELLO WORLD");
MODULE_VERSION("0.1");

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

void printGreeting(void){
	printk(KERN_ALERT "Hello World Driver loaded into kernel tree!");
}

int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;
	int i;
	for(dptr = dev->data; (dptr = next);)
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
	dev->quantum = QUANTUM;
	dev->qset = QSET;
	dev->data = NULL;
	return 0;
}

static irqreturn_t irq_handler(int irq, void *dev_id)
{
	printk(KERN_ALERT "Button pressed!\n");
	return IRQ_HANDLED;
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
	return 0;
}

struct scull_qset *scull_follow(struct scull_dev *dev, int n)
{
	printk(KERN_ALERT "scull_follow(): n=%d dev.quantum=%d dev.qset=%d dev.size=%lu\n", n, dev->quantum, dev->qset, dev->size);
	struct scull_qset *qs = dev->data;
	
	if(!qs){
		qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if(qs == NULL)
			return NULL;
		memset(qs, 0, sizeof(struct scull_qset));
	}
	
	while(n--){
		if(!qs->next){
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if(qs->next == NULL)
				return NULL;
			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
		continue;
	}
	return qs;
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
	
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	
	if(*f_pos >= dev->size)
	{
		goto out;
	}

	if(*f_pos + count > dev->size)
		count = dev->size - *f_pos;
	
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;
	
	dptr = scull_follow(dev, item);
	
	if(dptr == NULL ||!dptr->data || !dptr->data[s_pos])
		goto out;
		
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
		up(&dev->sem);
		return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_ALERT "The driver has been called with write()\n");
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr = dev->data;
	int quantum = dev->quantum, qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM;
	
	printk(KERN_ALERT "dev->size = %lu\n", dev->size);
	
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;
	
	printk(KERN_ALERT "item=%d s_pos=%d q_pos=%d rest=%d\n", item, s_pos, q_pos, rest);
	
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	
	dptr = scull_follow(dev, item);
	if(dptr == NULL)
		goto out;
	if(!dptr->data){
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if(!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	if(!dptr->data[s_pos]){
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if(!dptr->data[s_pos])
			goto out;
	}

	printk(KERN_ALERT "count=%ld\n", count);

	if(count > quantum - q_pos)
	{
		count = quantum - q_pos;
	}
	
	if(copy_from_user(dptr->data[s_pos] + q_pos, buf, count))
	{
		retval = -EFAULT;
		goto out;
	}

	*f_pos += count;
	retval = count;
	
	printk(KERN_ALERT "f_pos=%llu count=%ld\n", f_pos, count);
	
	if(dev->size < *f_pos)
		dev->size = *f_pos;

	out:
		up(&dev->sem);
		return retval;
}

static long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	char msgToUser[100] = "I am fine, thank you for asking!";
	char msgFromUser[100] = {0};
	int led_val;
	int ret_val, i, light = 1;
	printk(KERN_ALERT "ioctl called\n");
	if(_IOC_TYPE(cmd) != MYDRBASE) 
		return -EINVAL;
	switch(cmd){
		case SCULL_BLINK:
			if(!capable(CAP_SYS_ADMIN))
				return -EPERM;
			printk(KERN_ALERT "Blinking LED...\n");
			for(i=0;i<120;i++){
				gpio_set_value(LED, light);
				usleep_range(125000, 125001);
				light = !light;
			}
			return 1;
		break;
		case SCULL_BLINK_IOMEM:
			printk(KERN_ALERT "Turn LED on using memory-mapped IO\n");
			allocateIOMemory();
			led_val = GPIO_READ(LED);
			GPIO_OUTPUT(LED);
			printk(KERN_ALERT "led_val is %d\n", led_val);
			if(led_val == 0)
				GPIO_SET = 1 << LED;
			else
				GPIO_CLR = 1 << LED;
			ret_val = copy_to_user((char *)arg, msgToUser, sizeof(msgToUser));
			printk(KERN_ALERT "ret_val = %d\n", ret_val);
			deallocateIO(0);
		break;
		case SCULL_MESSAGE_FROM_USER:
			printk("Receiving message from user\n");
			ret_val = copy_from_user(msgFromUser, (char *)arg, sizeof(msgFromUser));
			printk(KERN_ALERT "ret_val=%d\n", ret_val);
			printk(KERN_ALERT "msgFromUser=%s\n", msgFromUser);
		break;
		default:
			return -ENOTTY;
	}
	return 0;
}

void allocateIOPort(void)
{
	printk(KERN_ALERT "Allocating GPIO button port...\n");
	int err, currentValue;
	err = gpio_is_valid(GPIO_BUTTON);
	if(err < 0)
		printk(KERN_ALERT "ERROR GPIO pin %d is invalid. ERRNO: %d\n", GPIO_BUTTON, err);
	
	err = gpio_request(GPIO_BUTTON, "Hello_World");
	if(err < 0)
		printk(KERN_ALERT "Failed requesting GPIO pin %d. ERRNO: %d\n", GPIO_BUTTON, err);
	
	err = gpio_direction_input(GPIO_BUTTON);
	if(err < 0)
		printk(KERN_ALERT "Can't set GPIO pin %d as input. ERRNO %d\n",GPIO_BUTTON, err);
}

void allocateIOMemory(void)
{
	char led_value = 'c';
	if(request_mem_region(GPIO_BASE, GPIO_LENGTH, "Acolla Molla's GPIO driver") == NULL)
	{
		printk(KERN_ALERT "ERROR: Can not allocate iomem for GPIO. But I dont care, lets try use the port anyway :)\n");
		//return;
	}
	printk(KERN_ALERT "iomem for GPIO is available. Proceeding to request mem region\n");
	gpio_device.map = ioremap(GPIO_BASE, GPIO_LENGTH);
	gpio_device.addr = (volatile unsigned int *)gpio_device.map;
	printk(KERN_ALERT "Has access to address starting at %u\n", gpio_device.addr);
	GPIO_INPUT(LED);

	//GPIO_OUTPUT(LED);
	//printk(KERN_ALERT "make GPIO_WRITE()\n");
	//GPIO_SET = 1 << LED;
	//printk(KERN_ALERT "Set GPIO pin %d to 1)\n", LED);
}

void deallocateIO(int type)
{
	if(type == 0)
	{
		release_mem_region(GPIO_BASE, GPIO_LENGTH);
		if(gpio_device.addr)
		{
			iounmap(gpio_device.addr);
		}
	}
	else if(type == 1)
	{
		release_mem_region(GPIO_BASE, GPIO_LENGTH);
		gpio_free(LED);
	}
}

int scull_release(struct inode *inode, struct file *filp)
{
	int irq_num;
	irq_num = gpio_to_irq(GPIO_BUTTON);
	free_irq(irq_num, NULL);
	return 0;
}

struct file_operations scull_fops =
{
	.owner = THIS_MODULE,
	.open = scull_open,
	.read = scull_read,
	.write = scull_write,
	.release = scull_release,
	.unlocked_ioctl = scull_ioctl,
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
	int result = 0, i=0;
	int irq_line, errno;
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
		scull_devices[i].quantum = QUANTUM;
		scull_devices[i].qset = QSET;
		sema_init(&scull_devices[i].sem, 1);
		scull_setup_cdev(&scull_devices[i], i);
	}

	printGreeting();
	allocateIOPort();
	//interrupt handler for gpio button
	irq_line = gpio_to_irq(GPIO_BUTTON);
	printk(KERN_ALERT "GPIO pin %d use IRQ Line %d\n", GPIO_BUTTON, irq_line);
	errno = request_irq(irq_line, (irq_handler_t)irq_handler, IRQF_TRIGGER_RISING, "GPIO_BTN", NULL);
	if(errno < 0)
	{
		printk(KERN_ALERT "Cant assign IRQ Line %d to GPIO pin %d\n", irq_line, GPIO_BUTTON);
	}
	//allocateIOMemory();
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
	/*if(gpio_device.addr)
	{
		iounmap(gpio_device.addr);
	}*/
	unregister_chrdev_region(dev, 1);
	//release_mem_region(GPIO_BASE, GPIO_LENGTH);
	//gpio_free(LED);
	printk(KERN_ALERT "Goodbye! Freeing MAJOR %d\n", dev_major);
}

module_init(hello_init);
module_exit(hello_exit);

