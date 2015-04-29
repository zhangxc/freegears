include $(TOPDIR)/.config

CFG_DEF_TARGET		= $(shell grep ^CONFIG_TARGET_.*=y $(TOPDIR)/.config | sed "s/CONFIG_TARGET_//" | sed "s/=y//")
CFG_DEF_ARCH		= $(shell grep ^CONFIG_ARCH_.*=y $(TOPDIR)/.config | sed "s/CONFIG_ARCH_//" | sed "s/=y//")
CFG_TOOL_PREFIX		= $(shell grep ^CONFIG_TOOLPREFIX_.*=y $(TOPDIR)/.config | sed "s/CONFIG_TOOLPREFIX_//" | sed "s/=y//")

CFG_DEFAULT_TARGET	= $(shell echo $(CFG_DEF_TARGET) | tr [:upper:] [:lower:])
CFG_DEFAULT_ARCH	= $(shell echo $(CFG_DEF_ARCH) | tr [:upper:] [:lower:])

#
CFG_TOOL_DIR	= $(TOPDIR)/tool
CFG_APP_DIR	= $(TOPDIR)/app
CFG_MOD_DIR	= $(TOPDIR)/module
CFG_BIN_DIR	= $(TOPDIR)/bin
CFG_DL_DIR	= $(TOPDIR)/dl

CFG_BUILD_DIR	= $(TOPDIR)/build
CFG_STG_DIR	= $(TOPDIR)/staging
CFG_TARGET_DIR	= $(TOPDIR)/target
HOST_BUILD_DIR	= $(CFG_BUILD_DIR)/host
HOST_STG_DIR	= $(CFG_STG_DIR)/host
HOST_TARGET_DIR	= $(CFG_TARGET_DIR)/host
TARGET_BUILD_DIR= $(CFG_BUILD_DIR)/$(CFG_DEFAULT_TARGET)
TARGET_STG_DIR	= $(CFG_STG_DIR)/$(CFG_DEFAULT_TARGET)
TARGET_DIR	= $(CFG_TARGET_DIR)/$(CFG_DEFAULT_TARGET)

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

# host variables
HOST_OS		= $(shell uname)
HOST_ARCH	= $(shell uname -m)
HOST_NAME	= $(shell gcc -dumpmachine)

HOST_AS		:= as
HOST_CC		:= gcc
HOST_CXX	:= g++
HOST_CPP	:= $(HOST_CC) -E
HOST_LD		:= ld
HOST_FC		:= f77
HOST_M2C	:= m2c
HOST_PC		:= pc
HOST_NM		:= nm
HOST_STRIP	:= strip
HOST_OBJCOPY	:= objcopy
HOST_OBJDUMP	:= objdump
HOST_RANLIB	:= ranlib
HOST_LDSHARED	:= gcc -shared

HOST_ASFLAGS	:=
HOST_CFLAGS	:= -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOST_CXXFLAGS	:= -O2
HOST_CPPFLAGS	:=
HOST_LDFLAGS	:=
HOST_LDLIBS	:=
HOST_FFLAGS	:=
HOST_PFLAGS	:=

# target variables
TARGET_ARCH		= $(CFG_DEFAULT_ARCH)
ifeq ($(CFG_TOOL_PREFIX), MIPS_UCLIBC_LINUX)
  TARGET_HOST		= mips-linux
  CROSS 		= $(TARGET_HOST)-
else
  ifeq ($(CFG_TOOL_PREFIX), ARM_GLIBC_LINUX)
    TARGET_HOST		= arm-hisiv200-linux
    CROSS 		= $(TARGET_HOST)-
  else
    TARGET_HOST		= $(shell gcc -dumpmachine)
    CROSS 		=
  endif
endif

TARGET_AS	:= $(CROSS)as
TARGET_CC	:= $(CROSS)gcc
TARGET_CXX	:= $(CROSS)g++
TARGET_CPP	:= $(TARGET_CC) -E
TARGET_LD	:= $(CROSS)ld
TARGET_FC	:= $(CROSS)f77
TARGET_M2C	:= $(CROSS)m2c
TARGET_PC	:= $(CROSS)pc
TARGET_NM	:= $(CROSS)nm
TARGET_STRIP	:= $(CROSS)strip
TARGET_OBJCOPY	:= $(CROSS)objcopy
TARGET_OBJDUMP	:= $(CROSS)objdump
TARGET_RANLIB	:= $(CROSS)ranlib
TARGET_LDSHARED	:= $(CROSS)gcc -shared

TARGET_ASFLAGS	:=
TARGET_CFLAGS	:= $(HOST_CFLAGS)
TARGET_CXXFLAGS	:=
TARGET_CPPFLAGS	:=
TARGET_LDFLAGS	:=
TARGET_LDLIBS	:=
TARGET_FFLAGS	:=
TARGET_PFLAGS	:=

