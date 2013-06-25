include ../Make.defines.in


TARGETS = utest list_test dltest mwtest conftest

all: $(TARGETS)

list_test: list_test.c

dltest: dltest.o ../lib/dlshared.so
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBFLAGS)

mwtest.o: mwtest.c
	$(CC) -c -o $@ $< $(CFLAGS) $(DBGFLAGS)

mwtest: mwtest.o ../lib/libmw.so

conftest: conftest.c 
	$(CC) -o $@ $< $(CFLAGS) $(LIBFLAGS)

# unit test code
utest.o: utest.c
	$(CC) -c -o $@ $< $(CFLAGS) $(DBGFLAGS)

utest: utest.o ../lib/libmw.so

clean:
	rm -f $(TARGETS) $(CLEANFILES) memwatch.log
