# //////////////////////////////////////////////////////////////////////////
# ///
# /// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
# ///
# //////////////////////////////////////////////////////////////////////////

UNAME	:= $(shell uname -r)
KDIR	:= /lib/modules/$(UNAME)/build
PWD	:= $(shell pwd)

MOD_TARGET = cidev
cidev-objs := ci_dev.o disk.o

obj-m	+= $(MOD_TARGET).o

all::
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean::
	$(MAKE) -C $(KDIR) M=$(PWD) clean
