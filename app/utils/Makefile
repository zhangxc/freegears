#
# Makefile of libfguitls
#

TOPDIR=../..
EXTRA_CFLAGS=-fPIC
EXTRA_CXXFLAGS=-fPIC
include $(TOPDIR)/app/config.mk

TARGET=libfgutils.so libfgutils.a libmw.so dlshared.so
LIBFGUTILS-objs	= abspath.o config.o strbuf.o usage.o wrapper.o eloop.o
LIBMW-objs	= memwatch.o
DLSHARED-objs	= dlshared.o

.PHONY: all build install clean distclean
all: build install

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
	rm -f *.o $(TARGET)

distclean: clean

