# HelloWorldDriver
A simple driver module for the Linux kernel that prints 'Hello world'. This project is following the tutorials in the book *Linux device drivers*.
First of all, install the Linux headers on the device:
```
sudo apt-get install build-essential linux-headers-$(uname -r)
```

Then run the Makefile from this project:
```
make
```

Initialize the compiled module using:
```
sudo ./scull_load
```
This script will automatically insert the module into the Kernel and create /dev/scull.

View the printk output by looking at the system logs (in Ubuntu):
```
tail /var/log/kern.log
```

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
