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

#define MYDRBASE 'k'
#define SCULL_RESET _IO(MYDRBASE, 0)
#define SCULL_GETSTATE _IO(MYDRBASE, 1)

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
