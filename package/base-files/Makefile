TOPDIR=$(CURDIR)/../..
include $(TOPDIR)/include/config.mk
include $(TOPDIR)/include/package.mk

PKG		:= base-files
PKG_NAME	:= base-files
PKG_RELEASE	:= 1
PKG_VERSION:=$(PKG_RELEASE)

define Package/base-files
SECTION:=base
CATEGORY:=Base system
TITLE:=Base filesystem for Freegear
URL:=http://what-the-hell
endef

define Package/base-files/conffiles
/etc/hosts
/etc/inittab
/etc/group
/etc/passwd
/etc/shadow
/etc/profile
/etc/shells
/etc/sysctl.conf
/etc/rc.local
/etc/sysupgrade.conf
/etc/config/
/etc/dropbear/
/etc/crontabs/
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)
Build/Install = $(Build/Compile/Default)

define Package/base-files/install
	$(CP) ./files/* $(1)/
	mkdir -p $(1)/CONTROL
	mkdir -p $(1)/dev
	mkdir -p $(1)/etc/crontabs
	mkdir -p $(1)/etc/rc.d
	mkdir -p $(1)/mnt
	mkdir -p $(1)/proc
	mkdir -p $(1)/tmp
	mkdir -p $(1)/usr/lib
	mkdir -p $(1)/usr/bin
	mkdir -p $(1)/sys
	mkdir -p $(1)/www
	mkdir -p $(1)/root
	ln -sf /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	ln -sf /tmp $(1)/var
	mkdir -p $(1)/etc
	ln -sf /tmp/resolv.conf /tmp/fstab /tmp/TZ $(1)/etc/

	chmod 0600 $(1)/etc/shadow
	chmod 1777 $(1)/tmp
endef

$(eval $(call BuildPackage,base-files))

