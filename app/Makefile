#
# third-party softwares
#
TOPDIR=..
include $(TOPDIR)/.config
include $(TOPDIR)/include/config.mk

# targets in built-in rules
SUBFATHER=
SUBDIRS= $(shell grep ^CONFIG_APP_.*=y $(TOPDIR)/.config | sed "s/CONFIG_APP_//" | sed "s/=y//")
OTHER_CMD_STOPED=y
include $(TOPDIR)/include/function.mk
include $(TOPDIR)/include/rules.mk

