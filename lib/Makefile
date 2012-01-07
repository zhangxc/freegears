include ../Make.defines.in

TARGET	= libfg.so dlshared.so memwatch.so

all: $(TARGET)

libfg.a: $(LIBFG_OBJS)
	ar rcs libfg.a $?
	ranlib libfg.a

libfg.so: $(LIBFG_OBJS)
	gcc -shared -o $@ $? 
	strip $@

dlshared.so: dlshared.o
	g++ -shared -o $@ $<
	strip $@

memwatch.o: memwatch.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
memwatch.so: memwatch.o
	gcc -shared -o $@ $<
	strip $@

clean:
	rm -f $(TARGET) $(CLEANFILES)
