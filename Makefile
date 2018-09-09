obj-m := test.o
KDIRBACK := /lib/modules/$(shell uname -r)/build
KDIR :=	/home/hwang/sdb/linux-4.4.13/
PWD := $(shell pwd)
MY_CFLAGS += -g -DDEBUG


defualt:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

debug:
	make -C $(KDIR) SUBDIRS=$(PWD) modules EXTRA_CFLAGS="$(MY_CFLAGS)"

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
		
