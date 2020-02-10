# HelloWorldDriver
A simple driver module for the Linux kernel. This project is following the tutorials in the book *Linux Device Drivers* and will evolve accordingly to the tasks in that book.

First of all, install the Linux headers on you're device:
```
sudo apt-get install build-essential linux-headers-$(uname -r)
```

Then run the Makefile from this project to compile *hello.c*:
```
make
```

Initialize the compiled module using:
```
sudo ./scull_load
```
This script will automatically insert the module into the Kernel and create the entry /dev/scull.

View the printk output by looking at the system logs (in Ubuntu):
```
tail -f /var/log/syslog
```
All printk() statements in the driver will be visible in the log.

Optionally, compile and run:
```
gcc test.c -o test
sudo ./test
```
The program *test* will make the call *open("/dev/scull")* which will trigger the *printk()* statement in the *scull_open()* function in hello.c and get printed to */var/log/kern.log*

Remove the inserted module using
```
sudo ./scull_unload
```
This script will automatically remove the module from the Kernel and delete /dev/scull.
