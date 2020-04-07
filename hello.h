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
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#define MYDRBASE 'k'
#define SCULL_BLINK _IO(MYDRBASE, 0)
#define SCULL_BLINK_IOMEM _IO(MYDRBASE, 1)
#define SCULL_MESSAGE_FROM_USER _IO(MYDRBASE, 2)
#define BCM2837_PERI_BASE 0x3F000000
#define GPIO_BASE (BCM2837_PERI_BASE + 0x200000)
#define GPIO_LENGTH 0xB3

const unsigned int LED = 16;
const unsigned int GPIO_BUTTON = 23;
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

struct gpio_dev
{
	void *map;
	volatile unsigned int *addr;
};
struct gpio_dev gpio_device = {GPIO_BASE};

//GPIO controller macros
#define GPIO_INPUT(g) *(gpio_device.addr + ((g)/10)) &= ~(7<<(((g) %10) *3))
#define GPIO_OUTPUT(g) *(gpio_device.addr + ((g)/10)) |= (1<<(((g) % 10) *3))
#define GPIO_SET *(gpio_device.addr + 7)
#define GPIO_READ(g) *(gpio_device.addr + 13) &= (1<<(g))
#define GPIO_CLR *(gpio_device.addr +10)

//Function prototypes
void allocateIOMemory(void);
void allocateIOPort(void);
void deallocateIO(int type);
