##############################################################################
# COPYRIGHT(C)	: Samsung Electronics Co.Ltd, 2006-2011 ALL RIGHTS RESERVED
# AUTHOR		: KyoungHOON Kim (khoonk)
##############################################################################
# VERSION&DATE	: Version 1.00	2006/06/02 (khoonk)
##############################################################################


EXTRA_CFLAGS += -I$(PRJROOT)/modules/include


obj-m		+= modemctl/
obj-m		+= onedram/
obj-m		+= svnet/

all:
	@$(MAKE) --no-print-directory -C $(KDIR) \
		SUBDIRS=$(CURDIR) modules

clean:
	rm -f *.ko *.mod.c *.mod.o *~ .*.cmd .*.o.cmd Module.symvers modules.order

install:
	@$(MAKE) --no-print-directory -C $(KDIR) \
		SUBDIRS=$(CURDIR) modules_install

