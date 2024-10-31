# Linked List
## Insmod Module to Raspberry Pi
```bash
make host
sudo insmod linked_list.ko
sudo chmod 666 /dev/rootv_dev
sudo echo 8 > /dev/rootv_dev
sudo cat /dev/rootv_dev
dmesg | tail -10
sudo rmmod linked_list
```
## Insmod Module to BeagleBone Black
### Ubuntu
```bash
make
sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
### BBB
```bash
make
sudo insmod linked_list.ko
sudo chmod 666 /dev/rootv_dev
sudo echo 8 > /dev/rootv_dev
sudo cat /dev/rootv_dev
dmesg | tail -10
sudo rmmod linked_list
```