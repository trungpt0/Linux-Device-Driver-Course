obj-m += skeleton.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabi-
KDIR=/home/bbb/BBB/image-builder/BBB/tmp/lib/modules/5.10.168/build
HDIR=/lib/modules/$(shell uname -r)/build

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

host:
	make -C $(HDIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

clean_host:
	make -C $(HDIR) M=$(PWD) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) help

help_host:
	make -C $(HDIR) M=$(PWD) help
