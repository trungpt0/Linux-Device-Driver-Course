# Run
## Raspberry
Let config Raspberry GPIO Pin
Mothod 1: Using button pull down and connect to gpio
```bash
make host
sudo insmod interrupt_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
```
Press button
```bash
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod interrupt_ldd
```
Mothod 2: Echo 1 and 0 to gpio
```bash
make host
sudo insmod interrupt_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo echo out > /sys/class/gpio/gpio49/direction
sudo echo 1 > /sys/class/gpio/gpio49/value
sudo echo 0 > /sys/class/gpio/gpio49/value
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod interrupt_ldd
```
## BeagleBone Black
### Ubuntu
```bash
make
sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
### BBB
Mothod 1: Using button pull down and connect to gpio
```bash
sudo insmod interrupt_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
```
Press button
```bash
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod interrupt_ldd
```
Mothod 2: Echo 1 and 0 to gpio
```bash
sudo insmod interrupt_ldd.ko
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo echo out > /sys/class/gpio/gpio49/direction
sudo echo 1 > /sys/class/gpio/gpio49/value
sudo echo 0 > /sys/class/gpio/gpio49/value
dmesg | tail -10
sudo cat /dev/rootv_dev
sudo rmmod interrupt_ldd
```