##############################################################################
# COPYRIGHT(C)	: Samsung Electronics Co.Ltd, 2006-2011 ALL RIGHTS RESERVED
# AUTHOR		: KyoungHOON Kim (khoonk)
##############################################################################
# VERSION&DATE	: Version 1.00	2006/06/02 (khoonk)
##############################################################################


EXTRA_CFLAGS += -I$(PRJROOT)/modules/include

obj-m		+= j4fs/
obj-m		+= param/
obj-m		+= vibetonz/
obj-m		+= multipdp/

#obj-$(CONFIG_FM_SI4709)         += fm_si4709/
#obj-$(CONFIG_STORAGE)                   += storage/
#obj-$(CONFIG_LEVEL)                     += level/
#obj-$(CONFIG_LTESPI)                    += ltespi/
#obj-$(CONFIG_SAMSUNG_DPRAM)             += dpram/
#obj-$(CONFIG_SAMSUNG_NVSTORAGE)         += nvstorage/

all:
	@$(MAKE) --no-print-directory -C $(KDIR) \
		SUBDIRS=$(CURDIR) modules

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *~ .*.cmd .*.o.cmd Module.symvers modules.order

install:
	@$(MAKE) --no-print-directory -C $(KDIR) \
		SUBDIRS=$(CURDIR) modules_install

