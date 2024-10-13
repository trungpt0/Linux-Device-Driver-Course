# Run
## Raspberry
```bash
make host
sudo insmod led.ko
dmesg | tail -10
sudo chmod 666 /dev/rootv_device
echo 1 > /dev/rootv_device
echo 0 > /dev/rootv_device
sudo rmmod led
```
## BeagleBone Black
### Ubuntu
```bash
make
sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
### BBB
```bash
sudo insmod led.ko
dmesg | tail -10
sudo chmod 666 /dev/rootv_device
echo 1 > /dev/rootv_device
echo 0 > /dev/rootv_device
sudo rmmod led
```
## App
```bash
gcc -o app app.c
./app
```