#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int f = open("/dev/scull", O_WRONLY);
	if(f < 0)
	{
		printf("Fail\n");
	}
	else
	{
		printf("Succeeded\n");
	}
	return 0;
}
