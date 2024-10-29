# Usage

Insert and remove kernel module

## Insert

```bash
sudo insmod chardd_dyna.ko
cat /proc/devices | grep "rootv_dev"
```

## Remove

```bash
sudo rmmod chardd_dyna
sudo dmesg | tail -10
```

# Authors

Trung Tran (RootV)

# License

GPL