#
# Makefile of libfguitls
#
#
# Ambigious APIs implementation:
#
#   strlcpy	libbsd, -lbsd
#   mkstemp	_BSD_SOURCE || _SVID_SOURCE
#
TOPDIR=../..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

.PHONY: all build install clean distclean
all: build install

TARGET	= libfgutils.so
CFLAGS	+= -fPIC
CXXFLAGS+= -fPIC
LDLIBS	+= -lrt

LIBFGUTILS-objs	= abspath.o config.o strbuf.o usage.o wrapper.o eloop.o \
		hashtable.o crc32.o
#		led.o wdog.o
#
LIBMW-objs	= memwatch.o

CUNIT-objs	= cunit.o

DLSHARED-objs	= dlshared.o

build: $(TARGET)

libfgutils.a: $(LIBFGUTILS-objs)
	$(AR) rcs libfg.a $?
	$(RANLIB) libfg.a

libfgutils.so: $(LIBFGUTILS-objs)
	$(LDSHARED) -o $@ $? $(LDFLAGS) $(LDLIBS)

dlshared.so: dlshared.o
	$(CXX) -shared -o $@ $<

memwatch.o: memwatch.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
libmw.so: memwatch.o
	$(CXX) -shared -o $@ $<

install:
	install -s --strip-program=$(STRIP) libfgutils.so $(CFG_TARGET_ROOT)/usr/lib

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET)

