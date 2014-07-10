TOPDIR=$(shell pwd)/..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

.PHONY: all prepare pkg_install tclib_install scripts_install config_install \
	https_app_install http_app_install

#all: prepare pkg_install tclib_install scripts_install config_install \
#	https_app_install http_app_install
all: prepare pkg_install

#
# Always need preparetion
#
PREP_PRE_DIRS	= $(CFG_ZAPP_INC_DIR) $(CFG_ZAPP_LIB_DIR) \
		  $(CFG_PACKAGE_SRC_DIR) $(CFG_PACKAGE_MKF_DIR)

ROOT_DIRS	= etc var dev/pts proc sys root kmod bin sbin mnt data \
		  usr/bin usr/sbin usr/lib usr/share usr/share/udhcpc \
		  lib/modules lib/firmware
CFG_DIRS	= usr/config-2.0.0
WEB_DIRS	= web
PREP_ROOT_DIRS	= $(addprefix $(CFG_TARGET_ROOT)/, $(ROOT_DIRS))
PREP_CFG_DIRS	= $(addprefix $(CFG_TARGET_ROOT)/, $(CFG_DIRS))
PREP_WEB_DIRS	= $(addprefix $(CFG_TARGET_ROOT)/, $(WEB_DIRS))
PREP_ROOTFS_DIRS= $(PREP_ROOT_DIRS) $(PREP_CFG_DIRS) $(PREP_WEB_DIRS)

PKG_BUILD_DIR   = package/build/$(ARCH)
PKG_ARCHIVE_DIR = package/archive/$(ARCH)
PREP_PKG_DIRS	= $(PKG_BUILD_DIR) $(PKG_ARCHIVE_DIR)

$(PREP_PRE_DIRS) $(PREP_ROOTFS_DIRS) $(PREP_PKG_DIRS):
	mkdir -p $@

pre_dirs_prepare: $(PREP_PRE_DIRS)

rootfs_prepare: $(PREP_ROOTFS_DIRS)

pkg_prepare: $(PREP_PKG_DIRS)

prepare: pre_dirs_prepare rootfs_prepare pkg_prepare

PKG_NAMES	= $(shell grep ^CONFIG_PKG_.*=y $(TOPDIR)/build/.config | sed "s/CONFIG_PKG_//" | sed "s/=y//")
PKG_PACKAGES	= $(PKG_NAMES:%=$(PKG_ARCHIVE_DIR)/%.tar.bz2)
ifneq ($(PKG_PACKAGES), )
$(PKG_PACKAGES):
	make -C $(PKG_BUILD_DIR) -f ../../makefiles/$(shell basename $@ | sed "s/.tar.bz2//").mk \
		PKG_BIN_BALL=../../../$@ || exit

pkg_install: pkg_prepare $(PKG_PACKAGES)
	for p in $(PKG_PACKAGES); do \
		tar xf $$p -C $(CFG_TARGET_ROOT); \
	done;
else
pkg_install:
endif

scripts_install: $(PREP_ROOTFS_DIRS)
	cp -rf script/default/* $(CFG_TARGET_ROOT)
	rm -f $(CFG_TARGET_ROOT)/usr/sbin/led_*

tclib_install: $(PREP_ROOTFS_DIRS)
	cp -a /opt/hisi-linux/x86-arm/arm-hisiv200-linux/target/armv7a_vfp_v3d16/lib/* ${CFG_TARGET_ROOT}/lib
	cp -a /opt/hisi-linux/x86-arm/arm-hisiv200-linux/arm-hisiv200-linux-gnueabi/lib/armv7a_vfp_v3d16/{libgcc_s.so*,libstdc++.so*} ${CFG_TARGET_ROOT}/lib

config_install: $(PREP_CFG_DIRS)
	cp -f config/default/* $(CFG_TARGET_ROOT)/usr/config-2.0.0/
	cp -f config/board/$(CONFIG_BOARD_TYPE)/product_info.conf $(CFG_TARGET_ROOT)/usr/share/product.conf
	cp -f config/board/$(CONFIG_BOARD_TYPE)/hardware.$(CONFIG_BOARD_TYPE) $(CFG_TARGET_ROOT)/usr/share/system.conf

https_app_install: $(PREP_WEB_DIRS)
	cp -rf web/default/* $(CFG_TARGET_ROOT)/web
	rm -f $(CFG_TARGET_ROOT)/web/.htpasswd
	ln -s /var/.htpasswd $(CFG_TARGET_ROOT)/web/.htpasswd
	rm -rf `find $(CFG_TARGET_ROOT)/web -name "*.bak"`
	cd $(CFG_TARGET_ROOT)/web; rename .html .tmpl *.html

http_app_install: data_build

data_build:
	cp -a data $(CFG_TARGET_ROOT)/usr

clean:

distclean: clean
	$(RM) -r $(CFG_TARGET_DIR)/image/rootfs.tar.gz
	$(RM) -r $(CFG_TARGET_DIR)/do-image/rootfs
	$(RM) -r $(CFG_TARGET_ROOT)

show:
	@echo $(CFG_DEFAULT_TARGET)
	@echo $(CFG_DEFAULT_ARCH)
	@echo $(CFG_TOOL_PREFIX)
	@echo $(TARGET)
	@echo $(ARCH)
	@echo $(CC)
