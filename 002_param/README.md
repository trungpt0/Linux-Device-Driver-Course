# How to Run Module?

## Host

Cross compile:

```bash
$ make
```

Send module and app to BBB:

```bash
$ sudo rsync -avz *.ko debian@192.168.7.2:/home/debian
```
## Target (BBB)

Insmod module:

```bash
$ sudo insmod param.ko
```

Check log:

```bash
$ dmesg | tail -10
```

Remove module:

```bash
$ sudo rmmod param
```