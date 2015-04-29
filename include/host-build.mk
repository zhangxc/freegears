include $(TOPDIR)/include/depends.mk

HOSTPKG_FILE_DIR	?= $(TOPDIR)/package/$(PKG_PATH)/files
HOSTPKG_SRC_DIR		?= $(TOPDIR)/package/$(PKG_PATH)/src
HOSTPKG_BUILD_DIR	?= $(HOST_BUILD_DIR)/$(PKG)-$(PKG_VERSION)
HOSTPKG_INS_DIR		?= $(HOSTPKG_BUILD_DIR)/host-install

HOSTPKG_STAMP_PREPARED	= $(HOSTPKG_BUILD_DIR)/.prepared_$(shell $(call find_md5,$(TOPDIR)/package/$(PKG_PATH) $(PKG_FILE_DEPENDS),))
HOSTPKG_STAMP_CONFIGURED= $(HOSTPKG_BUILD_DIR)/.configured
HOSTPKG_STAMP_BUILT	= $(HOSTPKG_BUILD_DIR)/.built
HOSTPKG_STAMP_INSTALLED	= $(HOST_TARGET_DIR)/stamp/.$(PKG_NAME)_installed

HOST_CONFIGURE_PREFIX	:= $(HOST_TARGET_DIR)/
HOST_CONFIGURE_VARS	= \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CXXFLAGS="$(HOST_CXXFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \

HOST_CONFIGURE_ARGS	= \
		--target=$(HOST_HOST) \
		--host=$(HOST_HOST) \
		--build=$(HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=$(HOST_CONFIGURE_PREFIX) \
		--exec-prefix=$(HOST_CONFIGURE_PREFIX) \
		--bindir=$(HOST_CONFIGURE_PREFIX)/bin \
		--sbindir=$(HOST_CONFIGURE_PREFIX)/sbin \
		--libexecdir=$(HOST_CONFIGURE_PREFIX)/lib \
		--sysconfdir=/etc \
		--datadir=$(HOST_CONFIGURE_PREFIX)/share \
		--localstatedir=/var \
		--mandir=$(HOST_CONFIGURE_PREFIX)/man \
		--infodir=$(HOST_CONFIGURE_PREFIX)/info \
	
HOST_MAKE_FLAGS =

HOST_CONFIGURE_CMD = ./configure

define Host/Prepare/Default
	tar xf $(CFG_DL_DIR)/$(PKG)-$(PKG_VERSION).tar.bz2 \
		-C $(HOST_BUILD_DIR)
	cd $(PKG_BUILD_DIR) && for p in $(PKG_PATCHES); do \
		patch -p1 -i $(HOSTPKG_SRC_DIR)/$$$$p; \
	done
endef

define Host/Prepare
  $(call Host/Prepare/Default)
endef

define Host/Configure/Default
	(cd $(HOSTPKG_BUILD_DIR)/$(3); \
		if [ -x configure ]; then \
			$(2) \
			$(HOST_CONFIGURE_CMD) \
			$(HOST_CONFIGURE_VARS) \
			$(HOST_CONFIGURE_ARGS) \
			$(1); \
		fi \
	)
endef

define Host/Configure
  $(call Host/Configure/Default)
endef

define Host/Compile/Default
	+$(MAKE) -C $(HOSTPKG_BUILD_DIR) \
		$(HOST_MAKE_FLAGS) \
		$(1)
endef

define Host/Compile
  $(call Host/Compile/Default)
endef

define Host/Install/Default
	$(MAKE) -C $(HOSTPKG_BUILD_DIR) \
		install DESTDIR=$(HOSTPKG_INS_DIR)
endef

define Host/Install
  $(call Host/Install/Default)
endef

.NOTPARALLEL:

define HostBuild
all: host-install
$(HOSTPKG_STAMP_PREPARED):
	@-rm -rf $(HOSTPKG_BUILD_DIR)
	@mkdir -p $(HOSTPKG_BUILD_DIR)
	$(call Host/Prepare)
	touch $$@

$(HOSTPKG_STAMP_CONFIGURED): $(HOSTPKG_STAMP_PREPARED)
	$(call Host/Configure)
	touch $$@

$(HOSTPKG_STAMP_BUILT): $(HOSTPKG_STAMP_CONFIGURED)
	$(call Host/Compile)
	touch $$@

$(HOSTPKG_STAMP_INSTALLED): $(HOSTPKG_STAMP_BUILT) $(if $(FORCE_HOSTPKG_INSTALL),FORCE)
	mkdir -p $(HOST_TARGET_DIR)/stamp
	$(call Host/Install)
	mkdir -p $$(shell dirname $$@)
	touch $$@

ifndef STAMP_BUILT
  prepare: host-prepare
  compile: host-compile
  install: host-install
  clean: host-clean
  update: host-update
  refresh: host-refresh
endif

  host-prepare: $(HOSTPKG_STAMP_PREPARED)
  host-configure: $(HOSTPKG_STAMP_CONFIGURED)
  host-compile: $(HOSTPKG_STAMP_BUILT) $(if $(STAMP_BUILT),$(HOSTPKG_STAMP_INSTALLED))
  host-install: $(HOSTPKG_STAMP_INSTALLED)
  host-clean: FORCE
	$(call Host/Clean)
	$(call Host/Uninstall)
	rm -rf $(HOSTPKG_BUILD_DIR) $(HOSTPKG_STAMP_INSTALLED) $(HOSTPKG_STAMP_BUILT)

endef

