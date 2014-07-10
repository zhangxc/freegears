# WE ARE NOW AT $(TOPDIR)/rootfs/package/build
TOPDIR=$(shell pwd)/../../../..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

PKG_NAME=$(shell basename $(PKG_BIN_BALL))
PKG=$(PKG_NAME:%.tar.bz2=%)
PKG_SRC_BALL=$(shell pwd)/../../storage/$(PKG_NAME)
PKG_SRC_DIR=$(shell pwd)/$(PKG)
PKG_INS_DIR=$(shell pwd)/$(PKG)/INSTALL_DIR

PKG_PATCHES	+= curl-7.34.0-clean.patch

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
	./configure --prefix=$(PKG_INS_DIR) \
		--host=$(CFG_TARGET_HOST) \
		--with-zlib=$(CFG_LIB_DIR) \
		--with-ssl\
		CC=$(CC) \
		CFLAGS=-I$(CFG_INC_DIR) \
		LDFLAGS="-L$(CFG_LIB_DIR) -lssl -lcrypto"


# 3. make
build: $(PKG) $(PKG)/Makefile
	make -C $(PKG)

# install targets in:
#   $(PKG)/INSTALL_DIR
#   $(CFG_INC_DIR) and $(CFG_LIB_DIR)
#   $(CFG_TARGET_ROOT)
install:
	make -C $(PKG) install
	$(RM) $(PKG_INS_DIR)/include/{README,Makefile*}
	$(RM) -r $(PKG_INS_DIR)/lib/{*.a,*.la,pkgconfig}
	$(RM) -r $(PKG_INS_DIR)/share
	$(RM) -r $(PKG_INS_DIR)/bin/curl-config
	$(STRIP) -s $(PKG_INS_DIR)/bin/curl
	$(STRIP) -s $(PKG_INS_DIR)/lib/*.so
	cp -a $(PKG_INS_DIR)/{include,lib} $(TOPDIR)
	$(RM) -r $(PKG_INS_DIR)/include

$(PKG_BIN_BALL):
	cd $(PKG_INS_DIR) && tar cjf $(shell pwd)/$(PKG_BIN_BALL) *

clean:

distclean:

