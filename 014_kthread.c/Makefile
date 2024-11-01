obj-m += kthread_ldd.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabi-

KDIR=/home/esacev/tmp/lib/modules/5.10.168/build
HDIR=/lib/modules/$(shell uname -r)/build

all: bbb

bbb:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

host:
	$(MAKE) -C $(HDIR) M=$(PWD) modules

clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

clean_host:
	$(MAKE) -C $(HDIR) M=$(PWD) clean

help:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) help

help_host:
	$(MAKE) -C $(HDIR) M=$(PWD) help
