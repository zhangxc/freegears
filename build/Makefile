#
# ZNAP mofi project
#
.PHONY = app
all: app

# Makefile configuration
TOPDIR=..
-include $(TOPDIR)/build/.config
include $(TOPDIR)/build/config.mk

# make function
include $(TOPDIR)/build/function.mk

# targets in built-in rules 
SUBFATHER=../
SUBDIRS=app
OTHER_CMD_STOPED=y
include $(TOPDIR)/build/rules.mk

#
# all
#
srctree		:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
objtree		:= $(CURDIR)
src		:= $(srctree)
obj		:= $(objtree)

VPATH		:= $(srctree)$(if $(KBUILD_EXTMOD),:$(KBUILD_EXTMOD))

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS = -O2

export srctree objtree VPATH

export VERSION PATCHLEVEL SUBLEVEL KERNELRELEASE KERNELVERSION
export ARCH SRCARCH CONFIG_SHELL HOSTCC HOSTCFLAGS CROSS_COMPILE AS LD CC
export CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK GENKSYMS INSTALLKERNEL PERL UTS_MACHINE
export HOSTCXX HOSTCXXFLAGS LDFLAGS_MODULE CHECK CHECKFLAGS

export KBUILD_CPPFLAGS NOSTDINC_FLAGS LINUXINCLUDE OBJCOPYFLAGS LDFLAGS
export KBUILD_CFLAGS CFLAGS_KERNEL CFLAGS_MODULE CFLAGS_GCOV
export KBUILD_AFLAGS AFLAGS_KERNEL AFLAGS_MODULE

# We need some generic definitions (do not try to remake the file).
$(srctree)/scripts/Kbuild.include: ;
include $(srctree)/scripts/Kbuild.include

PHONY	+= scripts_basic
scripts_basic:
	$(MAKE) $(build)=scripts/basic
	$(Q)rm -f .tmp_quiet_recordmcount

PHONY	+= outputmakefile
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# ===========================================================================
# *config targets only - make sure prerequisites are updated, and descend
# in scripts/kconfig to make the *config target

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration
# used for 'make defconfig'
KBUILD_KCONFIG=Wconfig
export KBUILD_DEFCONFIG KBUILD_KCONFIG

RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS -o -name .pc -o -name .hg -o -name .git \) -prune -o
export RCS_TAR_IGNORE := --exclude SCCS --exclude BitKeeper --exclude .svn --exclude CVS --exclude .pc --exclude .hg --exclude .git

config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

default:
	@echo "It's freezy ..."

#clean:
#	@find . $(RCS_FIND_IGNORE) \
#		\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
#		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
#		-o -name '*.symtypes' -o -name 'modules.order' \
#		-o -name modules.builtin -o -name '.tmp_*.o.*' \
#		-o -name '*.gcno' \) -type f -print | xargs rm -f
#
#distclean:
#	@find $(srctree) $(RCS_FIND_IGNORE) \
#		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
#		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
#		-o -name '.*.rej' -o -size 0 \
#		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
#		-type f -print | xargs rm -f

PHONY += FORCE
FORCE:

#.PHONY: $(PHONY)

