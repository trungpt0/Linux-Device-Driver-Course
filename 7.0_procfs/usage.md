# Usage

Insert and remove kernel module

## Insert

```bash
make
sudo insmod procfs_dev.ko
cat /proc/rootv/rootv_proc
sudo dmesg | tail -10
echo "Hi" > /proc/rootv/rootv_proc
sudo dmesg | tail -10
```

## Remove

```bash
sudo rmmod procfs_dev
sudo dmesg | tail -10
make clean
```

# Authors

Trung Tran (RootV)

# License

GPL