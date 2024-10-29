# Module name
obj-m += chardd_dyna.o

# ARM Cross-compilation settings
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabi-

# Kernel directories
KDIR=/home/bbb/BBB/image-builder/BBB/tmp/lib/modules/5.10.168/build
HDIR=/lib/modules/$(shell uname -r)/build

# Default target for cross-compilation
all: modules

# Build modules for target architecture
modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

# Build modules for host machine
host:
	$(MAKE) -C $(HDIR) M=$(PWD) modules

# Clean build files for target architecture
clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

# Clean build files for host machine
clean_host:
	$(MAKE) -C $(HDIR) M=$(PWD) clean

# Display help for target architecture
help:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) help

# Display help for host machine
help_host:
	$(MAKE) -C $(HDIR) M=$(PWD) help