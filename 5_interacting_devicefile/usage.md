# Usage

Insert and remove kernel module

## Insert

```bash
make
sudo insmod inter_df.ko
ls -l /dev | grep "rootv_device"
sudo dmesg | tail -10
sudo chmod 777 /dev/rootv_device
```
## App

```bash
make -f Makefile1
./app
```

## Remove

```bash
sudo rmmod inter_df
sudo dmesg | tail -10
make clean
make clean -f Makefile1
```

# Authors

Trung Tran (RootV)

# License

GPL