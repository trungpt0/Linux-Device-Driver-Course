# Usage

Insert and remove kernel module

## Insert

```bash
make
sudo insmod wq_dyna_dev.ko
sudo dmesg | tail -10
```

## Remove

```bash
sudo rmmod wq_dyna_dev
sudo dmesg | tail -10
make clean
```

# Authors

Trung Tran (RootV)

# License

GPL