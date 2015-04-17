include $(TOPDIR)/include/config.mk
#include $(TOPDIR)/include/ipkg.mk

export SH_FUNC	:=. $(TOPDIR)/scripts/shell.sh;

PKG_BUILD_DIR	= $(CURDIR)
PKG_INS_DIR	= $(CURDIR)/ipkg-install
STAMP_BUILT	= $(PKG_BUILD_DIR)/.built
STAMP_INSTALLED	= $(CFG_STAMP_DIR)/.$(PKG_NAME)_installed

CC		= $(CROSS_COMPILE)gcc
CPP		= $(CROSS_COMPILE)cpp
CXX		= $(CROSS_COMPILE)g++
CXXCPP		= $(CROSS_COMPILE)cpp -x c++
YACC		= $(CROSS_COMPILE)yacc
LD		= $(CROSS_COMPILE)ld
LDSHARED	= $(CROSS_COMPILE)gcc -shared
NM		= $(CROSS_COMPILE)nm
AR		= $(CROSS_COMPILE)ar
STRIP		= $(CROSS_COMPILE)strip
RANLIB		= $(CROSS_COMPILE)ranlib

CFLAGS		= $(CFG_TARGET_CFLAGS) -DCFG_VERSION_CODE=$(CFG_IMG_VERCODE) \
		  $(CFG_TARGET_OPTIMIZATION) -Wall \
		  -I$(CFG_STG_INC_DIR)
CPPFLAGS	=
CXXFLAGS	= $(CFLAGS)
YFLAGS		=
LDFLAGS		= -L$(CFG_STG_LIB_DIR)
LOADLIBES	=

HOSTCC		= gcc
HOSTCXX		= g++
HOSTCFLAGS	= -Os -Wall
INSTALL_BIN	= install -m755 -s --strip-program=$(STRIP)

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

