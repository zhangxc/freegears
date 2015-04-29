include $(TOPDIR)/include/depends.mk
#include $(TOPDIR)/include/ipkg.mk

INSTALL_BIN	= install -m755 -s --strip-program=$(STRIP)

PKG_BUILD_DIR	= $(CURDIR)
PKG_INS_DIR	= $(CURDIR)/ipkg-install
STAMP_BUILT	= $(PKG_BUILD_DIR)/.built
STAMP_INSTALLED	= $(CFG_STAMP_DIR)/.$(PKG_NAME)_installed

TARGET_CFLAGS	= $(CFG_TARGET_CFLAGS) -DCFG_VERSION_CODE=$(CFG_IMG_VERCODE) \
		  $(CFG_TARGET_OPTIMIZATION) -Wall \
		  -I$(CFG_STG_DIR)/$(CFG_DEFAULT_TARGET)/include
TARGET_CXXFLAGS	= $(TARGET_CFLAGS)
TARGET_LDFLAGS	= -L$(CFG_STG_DIR)/$(CFG_DEFAULT_TARGET)/lib


define Package/Default
CONFIGFILE:=
SECTION:=app
CATEGORY:=Application
DEPENDS:=
MDEPENDS:=
PROVIDES:=
EXTRA_DEPENDS:=
MAINTAINER:=easyit-dev@winder.net.cn
SOURCE:=app/$(shell basename $(CURDIR))
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

define Build/DefaultTargets
.PHONY: all compile install clean distclean
all: compile install
$(STAMP_BUILT): $(TARGET)
	touch $$@
compile: $(STAMP_BUILT)
%.o: %.c
	$(TARGET_CC) -c -o $$@ $$< $(TARGET_CFLAGS)
clean:
	$(RM) *.o $(STAMP_BUILT)
distclean: clean
	$(RM) -r $$(TARGET) ipkg-install/

$(call Package/$(1)/targets)
endef

define BuildApp
$(eval $(Package/Default))
$(eval $(Package/$(1)))
$(call Build/DefaultTargets,$(1))
$(call BuildTarget/ipkg,$(1))
endef

