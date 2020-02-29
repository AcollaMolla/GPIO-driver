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
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/gpio.h>

#define MYDRBASE 'k'
#define SCULL_RESET _IO(MYDRBASE, 0)
#define SCULL_GETSTATE _IO(MYDRBASE, 1)
#define SCULL_MESSAGE_FROM_USER _IO(MYDRBASE, 2)

const unsigned long START = 0x20200000;
const unsigned long LEN = 0x40;
const unsigned int LED = 27;

const int QUANTUM = 4000;
const int QSET = 1000;

static char *whom="world";
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
	struct semaphore sem;
	struct cdev cdev;
};
struct scull_dev *scull_devices;
