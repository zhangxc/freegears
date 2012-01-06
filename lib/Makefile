CFLAGS		+= -Os -Wall -fPIC
CXXFLAGS	+= $(CFLAGS)
LIBS_OBJS	= substr.o dlshared.o config.o abspath.o strbuf.o usage.o wrapper.o
LIBFG		= libfg.so

all: $(LIBFG)

libfg.a: $(LIBS_OBJS)
	ar rcs libfg.a $?
	ranlib libfg.a

# for dynamic link test
libfg.so: $(LIBS_OBJS)
	g++ -shared -o $@ $?
	strip $@

clean:
	rm -f $(LIBS_OBJS) $(LIBFG) libfg.so
