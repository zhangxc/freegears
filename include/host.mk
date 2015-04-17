include $(TOPDIR)/build/config.mk

CC		= gcc
CPP		= cpp
CXX		= g++
CXXCPP		= cpp -x c++
YACC		= yacc
LD		= ld
LDSHARED	= gcc -shared
NM		= nm
AR		= ar
STRIP		= strip
RANLIB		= ranlib

CFLAGS		= $(CFG_TARGET_CFLAGS) -DCFG_VERSION_CODE=$(CFG_IMG_VERCODE) -Wall
CPPFLAGS	=
CXXFLAGS	= $(CFLAGS)
YFLAGS		=
LDFLAGS		=
LOADLIBES	=

