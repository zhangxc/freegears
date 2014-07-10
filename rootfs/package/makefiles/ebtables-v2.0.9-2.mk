# WE ARE NOW AT $(TOPDIR)/rootfs/package/build
TOPDIR=$(shell pwd)/../../../..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

PKG_NAME=$(shell basename $(PKG_BIN_BALL))
PKG=$(PKG_NAME:%.tar.bz2=%)
PKG_SRC_BALL=$(shell pwd)/../../storage/$(PKG_NAME)
PKG_SRC_DIR=$(shell pwd)/$(PKG)
PKG_INS_DIR=$(shell pwd)/$(PKG)/INSTALL_DIR

PKG_PATCHES	+= ebtables-v2.0.9-2-root-install.patch

.PHONY: all build install
all: build install $(PKG_BIN_BALL)

# 1. extract
$(PKG):
	tar xf $(PKG_SRC_BALL)
	cd $(PKG) && for p in $(PKG_PATCHES); do \
		patch -p1 -i ../../../patches/$$p; \
	done

# 2. configure
#$(PKG)/Makefile:
#	cd $(PKG) && \
#	./configure --prefix=$(PKG_INS_DIR)

# 3. make
build: $(PKG) $(PKG)/Makefile
	make -C $(PKG) CC=$(CC) KERNEL_INCLUDES=$(CFG_KERNEL_DIR)/include

# install targets in:
#   $(PKG)/INSTALL_DIR
#   $(CFG_INC_DIR) and $(CFG_LIB_DIR)
#   $(CFG_TARGET_ROOT)
install:
	make -C $(PKG) install CC=$(CC) KERNEL_INCLUDES=$(CFG_KERNEL_DIR)/include \
		DESTDIR=$(PKG_INS_DIR)
	$(RM) -r $(PKG_INS_DIR)/{etc,share,man}
	$(STRIP) -s $(PKG_INS_DIR)/lib/*.so
	$(STRIP) -s $(PKG_INS_DIR)/sbin/{ebtables,ebtables-restore}
	cp -a $(PKG_INS_DIR)/lib $(TOPDIR)

$(PKG_BIN_BALL):
	cd $(PKG_INS_DIR) && tar cjf $(shell pwd)/$(PKG_BIN_BALL) *

clean:

distclean:

