#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>

#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOC_MSG _IO(SCULL_IOC_MAGIC, 1)
int main()
{
	int f = open("/dev/scull", O_RDWR);
	char msg[1000] = "A really long hello world message. Can it write more than 3 characters?";
	char buf[5] = "Hello";
	char buf1[5];
	size_t nbytes;
	ssize_t bytes_written=0;
	int fd;

	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
		printf("Trying ioctl()...\n");
		long ret_val = ioctl(f, SCULL_IOC_MSG, &buf1);
		printf("ioctl() returned: %ld", ret_val);
		printf("buf1 = %s\n", buf1);
		printf("Succeeded opening device\n Write you'r data to the device: ");
		fgets(msg, 1000, stdin);
		bytes_written = write(f, msg, sizeof(msg));
		if(bytes_written == sizeof(msg)/sizeof(msg[0]))
			printf("Wrote all %zu bytes successfully\n", bytes_written);
		else if(bytes_written > 0)
			printf("Wrote part of data\n");
		else if(bytes_written <= 0)
			printf("Failed writing to device\n");
		
	}
	return 0;
}
