#
CFG_DEFAULT_TARGET	= $(shell grep ^CONFIG_TARGET_.*=y $(TOPDIR)/build/.config | sed "s/CONFIG_TARGET_//" | sed "s/=y//")
CFG_DEFAULT_ARCH	= $(shell grep ^CONFIG_ARCH_.*=y $(TOPDIR)/build/.config | sed "s/CONFIG_ARCH_//" | sed "s/=y//")
CFG_TOOL_PREFIX		= $(shell grep ^CONFIG_TOOLPREFIX_.*=y $(TOPDIR)/build/.config | sed "s/CONFIG_TOOLPREFIX_//" | sed "s/=y//")

TARGET			= $(shell echo $(CFG_DEFAULT_TARGET) | tr [:upper:] [:lower:])
ARCH			= $(shell echo $(CFG_DEFAULT_ARCH) | tr [:upper:] [:lower:])
ifeq ($(CFG_TOOL_PREFIX), MIPS_UCLIBC_LINUX)
  CFG_TARGET_HOST	= mips-linux
  CROSS_COMPILE 	= $(CFG_TARGET_HOST)-
else
  ifeq ($(CFG_TOOL_PREFIX), ARM_GLIBC_LINUX)
    CFG_TARGET_HOST	= arm-hisiv200-linux
    CROSS_COMPILE 	= $(CFG_TARGET_HOST)-
  else
    CFG_TARGET_HOST	= $(shell gcc -dumpmachine)
    CROSS_COMPILE 	=
  endif
endif

#
CFG_BUILD_DIR	= $(TOPDIR)/build
CFG_TOOL_DIR	= $(TOPDIR)/tool
CFG_APP_DIR	= $(TOPDIR)/app
CFG_MOD_DIR	= $(TOPDIR)/module
CFG_BIN_DIR	= $(TOPDIR)/bin
CFG_INC_DIR	= $(TOPDIR)/include
CFG_LIB_DIR	= $(TOPDIR)/lib
CFG_TARGET_DIR	= $(TOPDIR)/target
CFG_TARGET_ROOT	= $(CFG_TARGET_DIR)/$(TARGET)/rootfs

CFG_PREFIX_DIR	= $(CFG_TARGET_DIR)
CFG_ROOT_DIR	= $(CFG_TARGET_DIR)

# builtin variables
AR		= ar
CO		= co
GET		= get
LEX		= lex
YACC		= yacc
LINT		= lint
MAKEINFO	= makeinfo
TEX		= tex
TEXI2DVI	= texi2dvi
WEAVE		= weave
CWEAVE		= cweave
TANGLE		= tangle
CTANGLE		= ctangle
RM		= rm -f

MAKEFLAGS	=
ARFLAGS		=
COFLAGS		=
GFLAGS		=
LFLAGS		=
YFLAGS		=
RFLAGS		=
LINTFLAGS	=

HOSTAS		= as
HOSTCC		= gcc
HOSTCXX		= g++
HOSTCPP		= $(HOSTCC) -E
HOSTLD		= ld
HOSTFC		= f77
HOSTM2C		= m2c
HOSTPC		= pc
HOSTNM		= nm
HOSTSTRIP	= strip
HOSTOBJCOPY	= objcopy
HOSTOBJDUMP	= objdump
HOSTRANLIB	= ranlib
HOSTLDSHARED	= gcc -shared

HOSTASFLAGS	=
HOSTCFLAGS	= -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS	= -O2
HOSTCPPFLAGS	=
HOSTLDFLAGS	=
HOSTLDLIBS	=
HOSTFFLAGS	=
HOSTPFLAGS	=

AS		= $(CROSS_COMPILE)as
CC		= $(CROSS_COMPILE)gcc
CXX		= $(CROSS_COMPILE)g++
CPP		= $(CC) -E
LD		= $(CROSS_COMPILE)ld
FC		= $(CROSS_COMPILE)f77
M2C		= $(CROSS_COMPILE)m2c
PC		= $(CROSS_COMPILE)pc
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
RANLIB		= $(CROSS_COMPILE)ranlib
LDSHARED	= $(CROSS_COMPILE)gcc -shared

ASFLAGS		=
CFLAGS		= $(HOSTCFLAGS)
CXXFLAGS	=
CPPFLAGS	=
LDFLAGS		=
LDLIBS		=
FFLAGS		=
PFLAGS		=

