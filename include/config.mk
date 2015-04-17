include $(TOPDIR)/.config

CFG_DEF_TARGET		= $(shell grep ^CONFIG_TARGET_.*=y $(TOPDIR)/.config | sed "s/CONFIG_TARGET_//" | sed "s/=y//")
CFG_DEF_ARCH		= $(shell grep ^CONFIG_ARCH_.*=y $(TOPDIR)/.config | sed "s/CONFIG_ARCH_//" | sed "s/=y//")
CFG_TOOL_PREFIX		= $(shell grep ^CONFIG_TOOLPREFIX_.*=y $(TOPDIR)/.config | sed "s/CONFIG_TOOLPREFIX_//" | sed "s/=y//")

CFG_DEFAULT_TARGET	= $(shell echo $(CFG_DEF_TARGET) | tr [:upper:] [:lower:])
CFG_DEFAULT_ARCH	= $(shell echo $(CFG_DEF_ARCH) | tr [:upper:] [:lower:])
ARCH			= $(CFG_DEFAULT_ARCH)
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
CFG_BUILD_DIR	= $(TOPDIR)/build/$(CFG_DEFAULT_TARGET)
CFG_TOOL_DIR	= $(TOPDIR)/tool
CFG_APP_DIR	= $(TOPDIR)/app
CFG_MOD_DIR	= $(TOPDIR)/module
CFG_BIN_DIR	= $(TOPDIR)/bin
CFG_STG_DIR	= $(TOPDIR)/staging
CFG_STG_BIN_DIR	= $(CFG_STG_DIR)/bin
CFG_STG_INC_DIR	= $(CFG_STG_DIR)/include
CFG_STG_LIB_DIR	= $(CFG_STG_DIR)/lib
CFG_TARGET_DIR	= $(TOPDIR)/target/$(CFG_DEFAULT_TARGET)
CFG_TARGET_ROOT	= $(CFG_TARGET_DIR)/rootfs
CFG_STAMP_DIR	= $(CFG_TARGET_DIR)/stamp

CFG_PREFIX_DIR	= $(CFG_TARGET_DIR)
CFG_ROOT_DIR	= $(CFG_TARGET_DIR)

# builtin variables
AR		:= ar
CO		:= co
GET		:= get
LEX		:= lex
YACC		:= yacc
LINT		:= lint
MAKEINFO	:= makeinfo
TEX		:= tex
TEXI2DVI	:= texi2dvi
WEAVE		:= weave
CWEAVE		:= cweave
TANGLE		:= tangle
CTANGLE		:= ctangle
RM		:= rm -f
INSTALL_BIN	:= install -m0755
INSTALL_SCRIPT	:= install -m0755
INSTALL_DIR	:= install -d -m0755
INSTALL_DATA	:= install -m0644
INSTALL_CONF	:= install -m0600

HOSTAS		:= as
HOSTCC		:= gcc
HOSTCXX		:= g++
HOSTCPP		:= $(HOSTCC) -E
HOSTLD		:= ld
HOSTFC		:= f77
HOSTM2C		:= m2c
HOSTPC		:= pc
HOSTNM		:= nm
HOSTSTRIP	:= strip
HOSTOBJCOPY	:= objcopy
HOSTOBJDUMP	:= objdump
HOSTRANLIB	:= ranlib
HOSTLDSHARED	:= gcc -shared

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

