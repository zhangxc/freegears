TOPDIR=$(CURDIR)/../..
include $(TOPDIR)/include/config.mk
include $(TOPDIR)/include/app.mk

TARGET_CFLAGS	+= -fPIC
TARGET_CXXFLAGS	+= -fPIC
TARGET_LDLIBS	+= -lrt
TARGET		= libfgutils.so libmw.so

define Package/libutils
PKG_NAME	:= libutils
PKG_VERSION	:= $(CFG_IMG_VER)
TITLE		:= Library of massive compact utilities
endef

define Package/libutils/targets
libfgutils.so: abspath.o config.o strbuf.o usage.o wrapper.o eloop.o \
		hashtable.o crc32.o
	$(TARGET_LDSHARED) -o $$@ $$^ $(TARGET_LDFLAGS) $(TARGET_LOADLIBES)

dlshared.so: dlshared.o
	$(TARGET_LDSHARED) -o $$@ $$<

memwatch.o: memwatch.c
	$(TARGET_CC) -c -o $$@ $$< $(TARGET_CFLAGS) -DMEMWATCH -DMW_STDIO
	
libmw.so: memwatch.o
	$(TARGET_CXX) -shared -o $$@ $$<
endef

define Package/libutils/install
	$(INSTALL_DIR) $(CFG_TARGET_ROOT)/usr/lib
	$(INSTALL_BIN) libfgutils.so $(CFG_TARGET_ROOT)/usr/lib
endef

$(eval $(call BuildApp,libutils))

