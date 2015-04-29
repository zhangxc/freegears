TOPDIR=$(CURDIR)/../../..
include $(TOPDIR)/include/config.mk
include $(TOPDIR)/include/package.mk
include $(TOPDIR)/include/host-build.mk

PKG		:= opkg
PKG_NAME	:= opkg
PKG_VERSION	:= 9c97d5ecd795709c8584e972bfdf3aee3a5b846d

define Package/opkg
SECTION		:= base
CATEGORY	:= Base system
TITLE		:= opkg package manager
MAINTAINER	:= Jo-Philipp Wich <xm@subsignal.org>
URL		:= http://wiki.openmoko.org/wiki/Opkg
endef

define Package/opkg/Default/description
  Lightweight package management system
  opkg is the opkg Package Management System, for handling
  installation and removal of packages on a system. It can
  recursively follow dependencies and download all packages
  necessary to install a particular package.

  opkg knows how to install both .ipk and .deb packages.
endef

define Package/opkg/conffiles
/etc/opkg.conf
endef

TARGET_CFLAGS += $(if $(CONFIG_GCC_VERSION_4_3)$(CONFIG_GCC_VERSION_4_4),-Wno-array-bounds)
TARGET_CFLAGS += -ffunction-sections -fdata-sections
EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CONFIGURE_ARGS += \
	--disable-curl \
	--disable-gpg \
	--with-opkgetcdir=/etc \
	--with-opkglockfile=/var/lock/opkg.lock

MAKE_FLAGS += \
	$(TARGET_CONFIGURE_OPTS) \
	DESTDIR="$(PKG_INS_DIR)" \
	HOST_CPU="$(ARCH)" \
	LDFLAGS="-Wl,--gc-sections" \

define Package/opkg/Default/install
	$(INSTALL_DIR) $(1)/usr/lib/opkg
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/opkg$(2).conf $(1)/etc/opkg.conf
  ifeq ($(CONFIG_PER_FEED_REPO),)
	echo "src/gz %n %U" >> $(1)/etc/opkg.conf
  else
	for d in base $(FEEDS_ENABLED); do \
		echo "src/gz %n_$$$$d %U/$$$$d" >> $(1)/etc/opkg.conf; \
	done
    ifneq ($(CONFIG_PER_FEED_REPO_ADD_DISABLED),)
	for d in $(FEEDS_DISABLED); do \
		echo "$(if $(CONFIG_PER_FEED_REPO_ADD_COMMENTED),# )src/gz %n_$$$$d %U/$$$$d" >> $(1)/etc/opkg.conf; \
	done
    endif
  endif
	$(VERSION_SED) $(1)/etc/opkg.conf
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/opkg-cl $(1)/bin/opkg
endef

Package/opkg/install = $(call Package/opkg/Default/install,$(1),)

HOST_CONFIGURE_ARGS+= \
	--disable-curl \
	--disable-gpg \
	--with-opkgetcdir=/etc \
	--with-opkglockfile=/tmp/opkg.lock

define Host/Install
	mkdir -p $(HOST_TARGET_DIR)/bin
	$(INSTALL_BIN) $(HOSTPKG_BUILD_DIR)/src/opkg-cl $(HOST_TARGET_DIR)/bin/opkg
endef

$(eval $(call BuildPackage,opkg))
$(eval $(call HostBuild,opkg))

