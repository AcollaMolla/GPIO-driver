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
