#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int main()
{
	int f = open("/dev/scull", O_RDWR);
	char msg[1000] = "A really long hello world message. Can it write more than 3 characters?";
	size_t nbytes;
	ssize_t bytes_written=0;
	int fd;

	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
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
