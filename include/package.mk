include $(TOPDIR)/include/depends.mk
#include $(TOPDIR)/include/ipkg.mk

export SH_FUNC		:= . $(TOPDIR)/scripts/shell.sh;

PKG_FILE_DIR		= $(TOPDIR)/package/$(PKG_PATH)/files
PKG_SRC_DIR		= $(TOPDIR)/package/$(PKG_PATH)/src
PKG_BUILD_DIR		= $(TARGET_BUILD_DIR)/$(PKG)-$(PKG_VERSION)
PKG_INS_DIR		= $(PKG_BUILD_DIR)/ipkg-install
STAMP_PREPARED		= $(PKG_BUILD_DIR)/.prepared_$(shell $(call find_md5,$(TOPDIR)/package/$(PKG_PATH) $(PKG_FILE_DEPENDS),))
STAMP_CONFIGURED	= $(PKG_BUILD_DIR)/.configured
STAMP_BUILT		= $(PKG_BUILD_DIR)/.built
STAMP_INSTALLED		= $(TARGET_DIR)/stamp/.$(PKG_NAME)_installed

TARGET_CFLAGS		+= $(CFG_TARGET_CFLAGS) $(CFG_TARGET_OPTIMIZATION) -Wall -I$(CFG_STG_DIR)/$(CFG_DEFAULT_TARGET)/include
TARGET_CXXFLAGS		+= $(TARGET_CFLAGS)
TARGET_LDFLAGS		+= -L$(TARGET_STG_DIR)/lib

TARGET_CONFIGURE_OPTS = \
  AS="$(TARGET_CC) -c $(TARGET_ASFLAGS)" \
  LD=$(TARGET_LD) \
  LDSHARED="$(TARGET_LDSHARED)" \
  NM=$(TARGET_NM) \
  CC="$(TARGET_CC)" \
  GCC="$(TARGET_CC)" \
  CXX="$(TARGET_CXX)" \
  RANLIB=$(TARGET_RANLIB) \
  STRIP=$(TARGET_STRIP) \
  OBJCOPY=$(CROSS)objcopy \
  OBJDUMP=$(CROSS)objdump \
  SIZE=$(CROSS)size

DISABLE_NLS		:= --disable-nls
DISABLE_LARGEFILE	:= --disable-large
DISABLE_IPV6		:= --disable-ipv6

CONFIGURE_PATH	= .
CONFIGURE_CMD	= ./configure
CONFIGURE_PREFIX:= /
CONFIGURE_VARS	= \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \

CONFIGURE_ARGS	= \
		--target=$(TARGET_HOST) \
		--host=$(TARGET_HOST) \
		--build=$(HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(CONFIGURE_PREFIX) \
		--exec-prefix=$(CONFIGURE_PREFIX) \
		--bindir=$(CONFIGURE_PREFIX)/bin \
		--sbindir=$(CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(CONFIGURE_PREFIX)/lib \
		--sysconfdir=/etc \
		--datadir=$(CONFIGURE_PREFIX)/share \
		--localstatedir=/var \
		--mandir=$(CONFIGURE_PREFIX)/man \
		--infodir=$(CONFIGURE_PREFIX)/info \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		$(DISABLE_IPV6)

MAKE_VARS = \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"

MAKE_FLAGS = \
	$(TARGET_CONFIGURE_OPTS) \
	CROSS="$(CROSS)" \
	ARCH="$(TARGET_ARCH)"

MAKE_INSTALL_FLAGS = \
	$(MAKE_FLAGS) \
	DESTDIR="$(PKG_INS_DIR)"

MAKE_PATH = .

#
# ipkg
#
define Package/Default
CONFIGFILE:=
SECTION:=opt
CATEGORY:=Extra packages
DEPENDS:=
MDEPENDS:=
PROVIDES:=
EXTRA_DEPENDS:=
MAINTAINER:=$(PKG_MAINTAINER)
SOURCE=package/$(PKG_PATH)
VERSION:=$(PKG_VERSION)
ABI_VERSION:=
ifneq ($(PKG_FLAGS),)
  PKGFLAGS:=$(PKG_FLAGS)
else
  PKGFLAGS:=
endif
ifneq ($(ARCH_PACKAGES),)
  PKGARCH:=$(ARCH_PACKAGES)
else
  PKGARCH:=$(BOARD)
endif
DEFAULT:=
MENU:=
SUBMENU:=
SUBMENUDEP:=
TITLE:=
KCONFIG:=
BUILDONLY:=
HIDDEN:=
URL:=
VARIANT:=
endef

define Build/Prepare/Default
	@$(RM) -r $(PKG_BUILD_DIR)
	tar xf $(CFG_DL_DIR)/$(PKG)-$(PKG_VERSION).tar.bz2 \
		-C $(TARGET_BUILD_DIR)/
	cd $(PKG_BUILD_DIR) && for p in $(PKG_PATCHES); do \
		patch -p1 -i $(PKG_SRC_DIR)/$$$$p; \
	done
endef

define Build/Configure/Default
	( cd $(PKG_BUILD_DIR); \
	if [ -x $(CONFIGURE_CMD) ]; then \
		$(CONFIGURE_VARS) \
		$(CONFIGURE_CMD) \
		$(CONFIGURE_ARGS); \
	fi; )
endef

define Build/Compile/Default
	+$(MAKE_VARS) \
		$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
endef

define Build/Install/Default
	+$(MAKE_VARS) \
		$(MAKE) -C $(PKG_BUILD_DIR) install $(MAKE_INSTALL_FLAGS)
endef

Build/Prepare=$(call Build/Prepare/Default,)
Build/Configure=$(call Build/Configure/Default,)
Build/Compile=$(call Build/Compile/Default,)
Build/Install=$(call Build/Install/Default,)

define Build/DefaultTargets
$(eval $(call rdep,$(CURDIR),$(STAMP_PREPARED),$(PKG_BUILD_DIR)/.dep_files,-x "*/.dep_*"))

$(STAMP_PREPARED):
	@-rm -rf $(PKG_BUILD_DIR)
	$(Build/Prepare)
	touch $$@

$(STAMP_CONFIGURED): $(STAMP_PREPARED)
	$(Build/Configure)
	rm -f $(STAMP_CONFIGURED_WILDCARD)
	touch $$@

$(STAMP_BUILT): $(STAMP_CONFIGURED)
	$(Build/Compile)
	$(Build/Install)
	touch $$@

$(STAMP_INSTALLED): $(STAMP_BUILT)
	mkdir -p $(TARGET_DIR)/stamp
	-[ -d $(PKG_FILE_DIR) ] && cp -a $(PKG_FILE_DIR)/* $(PKG_INS_DIR)
	-[ -d $(PKG_INS_DIR)/include ] && cp -a $(PKG_INS_DIR)/include $(TARGET_STG_DIR)
	-[ -d $(PKG_INS_DIR)/bin ] && cp -a $(PKG_INS_DIR)/bin $(TARGET_STG_DIR)
	-[ -d $(PKG_INS_DIR)/lib ] && cp -a $(PKG_INS_DIR)/lib $(TARGET_STG_DIR)
	touch $$@

compile: $(STAMP_INSTALLED)

endef

define BuildPackage
$(eval $(Package/Default))
$(eval $(Package/$(1)))
ifndef Package/$(1)/description
define Package/$(1)/description
	$(TITLE)
endef
endif

$(call shexport,Package/$(1)/description)
$(call shexport,Package/$(1)/config)

$(call Build/DefaultTargets,$(1))
ifeq ($(BLKPKG),)
$(call BuildTarget/ipkg,$(1))
endif
endef

all: compile

