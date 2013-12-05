# include ".config"


PWD_DIR		= $(shell pwd)
BUILD_DIR	= $(TOPDIR)/build
TOOL_DIR	= $(TOPDIR)/tool
INCLUDE_DIR	= $(TOPDIR)/include
LIB_DIR		= $(TOPDIR)/lib
TARGET_DIR	= $(TOPDIR)/target

CFG_PREFIX_DIR	= /home/wick/local
CFG_ROOT_DIR	=
CFG_TARGET_ARCH	= mips
CFG_TARGET_HOST	= mips-linux
#CROSS_COMPILE	:= $(CFG_TARGET_HOST)-
CROSS_COMPILE	:= 
