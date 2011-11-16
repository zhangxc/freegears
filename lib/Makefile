LIBS_OBJS	= substr.o

all: $(LIBS_OBJS)
	ar rv libfg.a $?
	strip -s libfg.a
	ranlib libfg.a

clean:
	rm -f $(LIBS_OBJS) libfg.a
