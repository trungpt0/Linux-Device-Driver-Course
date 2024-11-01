# Linked List
## Insmod Module to Raspberry Pi
```bash
make host
sudo insmod kthread_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod kthread_ldd
```
## Insmod Module to BeagleBone Black
### Ubuntu
```bash
make
sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
### BBB
```bash
sudo insmod kthread_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod kthread_ldd
```