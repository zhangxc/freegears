# Makefile configuration
TOPDIR=$(CURDIR)

# targets in built-in rules 
SUBFATHER=
SUBDIRS= rootfs app
OTHER_CMD_STOPED=y

#
# all (default) target
#
ifeq ($(wildcard .config), )
all: check_config
else
  include $(TOPDIR)/include/config.mk
  include $(TOPDIR)/include/pkg-path.mk
  PKG_PACKAGES	= $(shell grep ^CONFIG_PKG_.*=y .config | sed 's/CONFIG_PKG_//' | sed 's/=y//')

all: prepare pkg app
endif

#
# configuration
#
check_config: .config
.config: ./scripts/config/conf
	@+if [ \! -e .config ]; then \
	 	$(MAKE) menuconfig; \
	fi
		
scripts/config/conf:
	@$(MAKE) -s -C scripts/config conf

scripts/config/mconf:
	@$(MAKE) -s -C scripts/config mconf

config: scripts/config/conf FORCE
	$< Wconfig

defconfig: scripts/config/conf FORCE
	touch .config
	$< --defconfig=.config Wconfig

config-clean: FORCE
	$(MAKE) -c scripts/config clean

menuconfig: scripts/config/mconf FORCE
	$< Wconfig

distclean:
	$(MAKE) -C app clean
	$(MAKE) -C package clean
	$(RM) -r build staging target .config
	$(MAKE) -C scripts/config clean

#
# prepareation
#
$(HOST_BUILD_DIR) $(HOST_STG_DIR) $(HOST_TARGET_DIR) \
	$(TARGET_BUILD_DIR) $(TARGET_STG_DIR) $(TARGET_DIR):
	mkdir -p $@

prepare: $(HOST_BUILD_DIR) $(HOST_STG_DIR) $(HOST_TARGET_DIR) \
	 $(TARGET_BUILD_DIR) $(TARGET_STG_DIR) $(TARGET_DIR)

#
# pkg
#
pkg:
	make -C $(TOPDIR)/package

#
# app
#
app:
	make -C $(TOPDIR)/app

PHONY += FORCE
FORCE:

#.PHONY: $(PHONY)
.PHONY: all check_config prepare pkg app

