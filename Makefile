obj-m := test.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


defualt:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
		
