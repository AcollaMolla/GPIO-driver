# HelloWorldDriver
A simple driver module for the Linux kernel that prints 'Hello world'
First of all, install the Linux headers on the device:
```
sudo apt-get install build-essential linux-headers-$(uname -r)
```

Then run the Makefile from this project:
```
make
```

Insert the compiled module using
```
sudo insmod hello.ko
```

View the printk output by looking at the system logs (in Ubuntu):
```
tail /var/log/kern.log
```

Remove the inserted module using
```
sudo rmmod hello.ko
```
