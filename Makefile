CC		:= gcc
CXX		:= g++
CFLAGS		:= -Os -Wall -I./include 
CXXFLAGS	:= 

PROG		= list_test float_test poly
subdirs		= lib/ net/ unix/ utils/ memwatch/

all:	$(PROG)
	for dir in $(subdirs); do	\
		(cd $$dir; make)	\
	done

list_test: list_test.c

float_test: float_test.c
	$(CC) $(CFLAGS) -o $@ $< -lm

poly: poly.cpp

clean:
	for dir in $(subdirs); do	\
		(cd $$dir; make $@)	\
	done
	rm -f $(PROG) *.o *~
