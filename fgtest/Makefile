include ../Make.defines.in


TARGETS = list_test dltest mwtest conftest

all: $(TARGETS)

list_test: list_test.c

dltest: dltest.o ../lib/dlshared.so
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBFLAGS)

mwtest.o: mwtest.c
	$(CC) -c -o $@ $< $(CFLAGS) -DMEMWATCH -DMW_STDIO

mwtest: mwtest.o ../lib/memwatch.so

conftest: conftest.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBFLAGS)

clean:
	rm -f $(TARGETS) $(CLEANFILES) memwatch.log
