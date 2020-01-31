#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main()
{
	FILE *f;
	f = fopen("/dev/scull","w");
	if(f == NULL || f < 0)
	{
		printf("Fail\n");
		fprintf(stderr, "fopen() failed: %s\n", strerror(errno));
	}
	else
	{
		printf("Succeeded and pointing at %p\n", f);
	}
	return 0;
}
