# Run
## Raspberry
```bash
make host
sudo insmod sysfs_ldd.ko
sudo cat /sys/kernel/rootv_sysfs/value
sudo echo 1 > /sys/kernel/rootv_sysfs/value
sudo cat /sys/kernel/rootv_sysfs/value
dmesg | tail -10
sudo rmmod sysfs_ldd
```
## BeagleBone Black
### Ubuntu
```bash
make
sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
### BBB
```bash
sudo insmod sysfs_ldd.ko
sudo cat /sys/kernel/rootv_sysfs/value
sudo echo 1 > /sys/kernel/rootv_sysfs/value
sudo cat /sys/kernel/rootv_sysfs/value
dmesg | tail -10
sudo rmmod sysfs_ldd
```