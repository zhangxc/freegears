#
# Makefile of libfguitls
#

TOPDIR=../..
include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

.PHONY: all build install clean distclean
all: build install

TARGET	= libfgutils.so libfgutils.a libmw.so dlshared.so
CFLAGS	+= -fPIC
CXXFLAGS+= -fPIC

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
	$(LDSHARED) -o $@ $? -lbsd
	$(STRIP) $@

dlshared.so: dlshared.o
	$(CXX) -shared -o $@ $<
	$(STRIP) $@

memwatch.o: memwatch.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
libmw.so: memwatch.o
	$(CXX) -shared -o $@ $<
	$(STRIP) $@

install:

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET)

