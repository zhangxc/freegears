#
#
#
ifeq ($(V), 1)
Q=
else
Q=@
endif

ifndef lowercase
lowercase = $(shell echo $1 | sed -e "y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/")
endif
ifndef uppercase
uppercase = $(shell echo $1 | sed -e "y/abcdefghijklmnopqrstuvwxy/zABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/")
endif

CDCMD=@echo Making $@ && cd $@ &&
