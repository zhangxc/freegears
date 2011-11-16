# kernel modules makefile
# dependencies: /usr/src/linux directory

obj-m		+= hello.o
#hello-objs	:= file1.o file2.o

all:
	make -C /usr/src/linux M=$(shell pwd) modules

clean:
	make -C /usr/src/linux M=$(shell pwd) clean

