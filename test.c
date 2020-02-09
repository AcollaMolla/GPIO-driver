#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main()
{
	int f = open("/dev/scull", O_RDONLY);
	char buff[1000];
	size_t nbytes;
	ssize_t bytes_read=0;
	int fd;

	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
		printf("Succeeded opening device\n");
		nbytes = sizeof(buff);
		bytes_read = read(f, buff, nbytes);
		if(bytes_read == 0)
		{
			printf("No data on device\n");
		}
		else
			printf("The device content: %s\n", buff);
	}
	return 0;
}
