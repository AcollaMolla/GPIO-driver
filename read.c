#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int main()
{
	char buff[1000];
	size_t nbytes;
	ssize_t bytes_read=0;
	
	int f = open("/dev/scull", O_RDONLY);
	
	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
		nbytes = sizeof(buff);
		bytes_read = read(f, buff, nbytes);
		if(bytes_read == 0)
		{
			printf("No data on device (%zu)\n", bytes_read);
		}
		else
			printf("The device content: %s\n", buff);
	}
	return 0;
}
