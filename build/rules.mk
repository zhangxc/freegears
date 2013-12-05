#
# 
#

_PHONY=build depend dep $(SUBDIRS) .clean clean distclean
#_PHONY+= .pkg_include .spkg_include .module_include .app_include .tool_include .3party_include
#_PHONY+= kernelprepare
.PHONY: $(_PHONY)
.SUFFIXES:

OTHER_CMDS=$(filter-out $(SUBDIRS),$(filter-out $(_PHONY),$(MAKECMDGOALS)))

build: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $(SUBFATHER)$@

depend dep:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $(SUBFATHER)$$dir .depend || exit ; done

.clean:
	$(RM) -f *.a *.so *.o core core.* .depend

clean:: .clean
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $(SUBFATHER)$$dir clean || exit ; done

distclean::
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $(SUBFATHER)$$dir distclean || exit ; done

ifneq ($(OTHER_CMD_STOPED),y)
$(OTHER_CMDS)::                                                                                         
	$(Q)for dir in $(SUBDIRS); do $(MAKE) -C $(SUBFATHER)$$dir $@ || exit ; done
endif
