#
# vars.mk - Makefile tricks to debug Makefile variables
#
#   make -f test.mk -f vars.mk VAR
#   make -f test.mk -f vars.mk d-VAR
#
# d-output --
#
#   origin: file / environment / override / environment override
#   		/ command line / automatic
#
#    value: (prototype before folded)
#
#   flavor: simple / recursive
#

%:
	@echo '$*=$($*)'

d-%:
	@echo '$*=$($*)'
	@echo '  origin = $(origin $*)'
	@echo '   value = $(value  $*)'
	@echo '  flavor = $(flavor $*)'

