include ../Make.defines.in


TARGETS = list_test dltest mwtest

all: $(TARGETS)

list_test: list_test.c

dltest: dltest.o ../lib/dlshared.so
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBFLAGS)

mwtest.o: mwtest.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO
	
mwtest: mwtest.o ../lib/memwatch.so

clean:
	rm -f $(TARGETS) $(CLEANFILES) memwatch.log
