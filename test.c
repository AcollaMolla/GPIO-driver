#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int main()
{
	int f = open("/dev/scull", O_RDWR);
	char buff[1000];
	size_t nbytes;
	ssize_t bytes_read;
	int fd;

	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
		printf("Succeeded\n");
		nbytes = sizeof(buff);
		bytes_read = read(fd, buff, nbytes);
	}
	return 0;
}
