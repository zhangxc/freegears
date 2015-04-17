TOPDIR=$(CURDIR)/../..
include $(TOPDIR)/include/app.mk

CFLAGS	+= -fPIC
CXXFLAGS+= -fPIC
LDLIBS	+= -lrt
TARGET	= libfgutils.so libmw.so

define Package/libutils
PKG_NAME	:= libutils
PKG_VERSION	:= $(CFG_IMG_VER)
TITLE		:= Library of massive compact utilities
endef

define Package/libutils/targets
libfgutils.so: abspath.o config.o strbuf.o usage.o wrapper.o eloop.o \
		hashtable.o crc32.o
	$(LDSHARED) -o $$@ $$^ $(LDFLAGS) $(LOADLIBES)

dlshared.so: dlshared.o
	$(LDSHARED) -o $$@ $$<

memwatch.o: memwatch.c
	$(CC) -c -o $$@ $$< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
libmw.so: memwatch.o
	$(CXX) -shared -o $$@ $$<
endef

define Package/libutils/install
	$(INSTALL_DIR) $(CFG_TARGET_ROOT)/usr/lib
	$(INSTALL_BIN) libfgutils.so $(CFG_TARGET_ROOT)/usr/lib
endef

$(eval $(call BuildApp,libutils))

