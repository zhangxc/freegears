include ../Make.defines.in

CFLAGS += -fPIC
TARGET	= libfg.so dlshared.so libmw.so

all: $(TARGET)

libfg.a: $(LIBFG_OBJS)
	ar rcs libfg.a $?
	ranlib libfg.a

libfg.so: $(LIBFG_OBJS)
	gcc -shared -o $@ $? -lbsd
	strip $@

dlshared.so: dlshared.o
	g++ -shared -o $@ $<
	strip $@

memwatch.o: memwatch.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
libmw.so: memwatch.o
	gcc -shared -o $@ $<
	strip $@

clean:
	rm -f $(TARGET) $(CLEANFILES)
