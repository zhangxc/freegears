CC		:= gcc
CXX		:= g++
CFLAGS         := -Os -Wall -I./include 
CXXFLAGS	:= $(CFLAGS)
LIBFLAGS	:= -L$(shell pwd)/lib -lfg -lm -ldl

PROG		= list_test float_test poly dltest
DIRS		= lib/ net/ unix/ utils/ memwatch/

all: subdirs $(PROG)
	
subdirs:
	for dir in $(DIRS); do	\
		(cd $$dir; make)	\
	done

list_test: list_test.c

float_test: float_test.c
	$(CC) -o $@ $< $(LIBFLAGS)

poly: poly.cpp

# explicit dynamic link test, check lib/dlshared.cpp for details
dltest: dltest.cpp lib/libfg.so
	$(CXX) -o $@ $< $(LIBFLAGS)

clean:
	for dir in $(DIRS); do	\
		(cd $$dir; make $@)	\
	done
	rm -f $(PROG) *.o *~
