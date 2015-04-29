TOPDIR=..
include $(TOPDIR)/include/config.mk
include $(TOPDIR)/include/pkg-path.mk

# targets in built-in rules
SUBFATHER	=
SUBDIRS		= $(package-y)

.PHONY: $(SUBDIRS) clean distclean
build: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $(SUBFATHER)$@ PKG_PATH=$@

clean:
distclean:

