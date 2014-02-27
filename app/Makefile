#
# third-party softwares
#
TOPDIR=..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

# make function
include $(TOPDIR)/build/function.mk

# targets in built-in rules
SUBFATHER=
SUBDIRS= $(shell grep ^CONFIG_APP_.*=y $(TOPDIR)/build/.config | sed "s/CONFIG_APP_//" | sed "s/=y//")
OTHER_CMD_STOPED=y
include $(TOPDIR)/build/rules.mk

#
# overrides the rules in rules.mk
#
.PHONY: build clean distclean $(SUBDIRS)
build: $(SUBDIRS)

# app
# libraries
# python package

