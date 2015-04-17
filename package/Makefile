TOPDIR=..
include $(TOPDIR)/include/config.mk
include $(TOPDIR)/include/pkg-path.mk

# targets in built-in rules
SUBFATHER	=
SUBDIRS		= $(package-y)
OTHER_CMD_STOPED=y

.PHONY: $(SUBDIRS) clean distclean

$(SUBDIRS):
	$(MAKE) -C $(SUBFATHER)$@ PKG_PATH=$@

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $(SUBFATHER)$$dir clean || exit ; \
	done

distclean:
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) -C $(SUBFATHER)$$dir distclean || exit ; \
	done
