# How to Run Module?

## Host

Cross compile:

```bash
$ make
```

Send module and app to BBB:

```bash
$ sudo rsync -avz *.ko signal_app.c debian@192.168.7.2:/home/debian
```
## Target (BBB)

Compile app:

```bash
$ gcc -o signal_app signal_app.c
```

Insmod module:

```bash
$ sudo insmod signal_ldd.ko
```

Check log:

```bash
$ dmesg | tail -10
```

Add permission for device file:

```bash
$ sudo chmod 666 /dev/rootv_device
```

Run app:

```bash
$ ./signal_app
```

Using another tab and trigger:

```bash
$ echo out > /sys/class/gpio/gpio30/direction
$ echo 1 > /sys/class/gpio/gpio30/value
$ echo 0 > /sys/class/gpio/gpio30/value
```

Let's see app:

```bash
$ ./signal_app
received signal: 44
```

Remove module:

```bash
$ sudo rmmod signal_ldd
```