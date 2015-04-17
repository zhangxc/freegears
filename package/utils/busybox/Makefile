# WE ARE NOW AT $(TOPDIR)/rootfs/package/build
TOPDIR=$(CURDIR)/../../..
include $(TOPDIR)/include/package.mk

LOADLIBES	+= -lm

define Package/busybox
PKG		:= busybox
PKG_NAME	:= busybox
PKG_VERSION	:= 1.22.1
SECTION		:= base
CATEGORY	:= Base system
TITLE		:= Core utilities for embedded Linux
URL		:= http://busybox.net
DEPENDS		:= +BUSYBOX_USE_LIBRPC:librpc +BUSYBOX_CONFIG_PAM:libpam

PKG_PATCHES	=
endef

define Package/busybox/conffiles
endef

define Build/Configure
	rm -f $(PKG_BUILD_DIR)/.configured*
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(CC)" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		ARCH="$(ARCH)" \
		defconfig
endef

define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(ARCH)" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		SKIP_STRIP=y \
		all
endef

define Build/Install
	$(RM) -r $(PKG_INS_DIR)
	find $(PKG_BUILD_DIR) -lname "*busybox" -exec rm \{\} \;
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(ARCH)" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		CONFIG_PREFIX="$(PKG_INS_DIR)" \
		install
endef

$(eval $(call BuildPackage,busybox))

