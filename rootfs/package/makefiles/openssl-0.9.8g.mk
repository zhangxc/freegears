# WE ARE NOW AT $(TOPDIR)/rootfs/package/build
TOPDIR=$(shell pwd)/../../../..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

PKG_NAME=$(shell basename $(PKG_BIN_BALL))
PKG=$(PKG_NAME:%.tar.bz2=%)
PKG_SRC_BALL=$(shell pwd)/../../storage/$(PKG_NAME)
PKG_SRC_DIR=$(shell pwd)/$(PKG)
PKG_INS_DIR=$(shell pwd)/$(PKG)/INSTALL_DIR

PKG_PATCHES	+= openssl-0.9.8g-embedded-support.patch

ifeq ($(strip $(CONFIG_TARGET_APACHE_PEAK)), y)
OPENSSL_PROFILE=linux-mips32r2
else
OPENSSL_PROFILE=no-asm
endif

.PHONY: all build install
all: build install $(PKG_BIN_BALL)

# 1. extract
$(PKG):
	tar xf $(PKG_SRC_BALL)
	cd $(PKG) && for p in $(PKG_PATCHES); do \
		patch -p1 -i ../../../patches/$$p; \
	done

# 2. configure
$(PKG)/Makefile:
	cd $(PKG) && \
	./Configure $(OPENSSL_PROFILE) shared --prefix=$(PKG_INS_DIR)

# 3. make
build: $(PKG) $(PKG)/Makefile
	make -C $(PKG) CC=$(CC)

# install targets in:
#
#   $(PKG)/INSTALL_DIR
#   $(CFG_INC_DIR) and $(CFG_LIB_DIR)
#   $(CFG_TARGET_ROOT)
install:
	make -C $(PKG) install
	chmod +w $(PKG_INS_DIR)/include/openssl/*.h
	chmod +w $(PKG_INS_DIR)/lib/libcrypto.so.0.9.8
	chmod +w $(PKG_INS_DIR)/lib/libssl.so.0.9.8
	chmod +w $(PKG_INS_DIR)/lib/libcrypto.a
	chmod +w $(PKG_INS_DIR)/lib/libssl.a
	chmod +w $(PKG_INS_DIR)/lib/engines/*.so
	$(STRIP) -s $(PKG_INS_DIR)/bin/openssl
	$(STRIP) -s $(PKG_INS_DIR)/lib/*.so
	$(STRIP) -s $(PKG_INS_DIR)/lib/engines/*.so
	$(RM) -r $(PKG_INS_DIR)/lib/{*.a,pkgconfig}
	$(RM) -r $(CFG_INC_DIR)/openssl
	cp -a $(PKG_INS_DIR)/{include,lib} $(TOPDIR)
	$(RM) -r $(PKG_INS_DIR)/{include,ssl}

$(PKG_BIN_BALL):
	cd $(PKG_INS_DIR) && tar cjf $(shell pwd)/$(PKG_BIN_BALL) *

clean:

distclean:

