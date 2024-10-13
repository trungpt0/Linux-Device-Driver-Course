# Usage

Insert and remove kernel module

## Insert

```bash
sudo insmod dfile.ko
ls -l /dev | grep "rootv_device"
```

## Remove

```bash
sudo rmmod dfile
sudo dmesg | tail -10
```

# Authors

Trung Tran (RootV)

# License

GPL