#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>

#define SCULL_IOC_MAGIC 'k'
#define SCULL_BLINK _IO(SCULL_IOC_MAGIC, 0)
#define SCULL_GETSTATE _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_MESSAGE_FROM_USER _IO(SCULL_IOC_MAGIC, 2)

int main()
{
	int f = open("/dev/scull", O_RDWR);
	char msg[100] = "Hello, I am a user space process!";
	char buf[100] = {0};
	char val='n';
	long ret_val = 0;
	size_t nbytes;
	ssize_t bytes_written=0;
	int fd;

	if(f < 0)
	{
		printf("Failed to open device\n");
	}
	else
	{
		do{
			printf("Make a choice:\n");
			printf("(0) Make LED blink\n");
			printf("(1) Receive driver state\n");
			printf("(2) Send message to driver\n");
			printf("(9) Continue...\n");
			scanf("%c%*c", &val);
			switch(val){
				case '0':
					printf("The LED will blink for 20 sec...");
					ret_val = ioctl(f, SCULL_BLINK);
					if(ret_val == 123)printf("Finished blinking!\n");
					else printf("Cant't make the LED blink...\n");
				break;
			
				case '1':
					printf("Asking device for status\n");
					ret_val = ioctl(f, SCULL_GETSTATE, &buf);
					printf("Received status: %s\n", buf);
				break;
			
				case '2':
					printf("Sending message to driver\n");
					ret_val = ioctl(f, SCULL_MESSAGE_FROM_USER, &msg);
					if(ret_val == 0)printf("Successfully sent message!\n"); 
					else if(ret_val > 0)printf("Sent message partially\n");
					else if(ret_val < 0)printf("Canät send message...\n");
				break;
			}
		}while(val != '9');
		
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
