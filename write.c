#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>

#define SCULL_IOC_MAGIC 'k'
#define SCULL_GETSTATE _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_MESSAGE_FROM_USER _IO(SCULL_IOC_MAGIC, 2)

int main()
{
	int f = open("/dev/scull", O_RDWR);
	char msg[1000] = "A really long hello world message. Can it write more than 3 characters?";
	char buf[100] = {0};
	char val='n';
	size_t nbytes;
	ssize_t bytes_written=0;
	int fd;

	if(f < 0)
	{
		printf("Failed to open device\n");
	}
	else
	{
		printf("Asking device for status\n");
		long ret_val = ioctl(f, SCULL_GETSTATE, &buf);
		printf("Received status: %s\n", buf);
		printf("Do you wan't to write to device? y/n: ");
		scanf("%c%*c", &val);
		if(val == 'y'){
			printf("Write you'r data to the device: ");
			fgets(msg, 1000, stdin);
			bytes_written = write(f, msg, sizeof(msg));
			if(bytes_written == sizeof(msg)/sizeof(msg[0]))
				printf("Wrote all %zu bytes successfully\n", bytes_written);
			else if(bytes_written > 0)
				printf("Wrote part of data\n");
			else if(bytes_written <= 0)
				printf("Failed writing to device\n");
		}
		
	}
	return 0;
}
