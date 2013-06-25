include ./Make.defines.in

all:
	@echo "Nothing to make in root directory"
	@echo "Please read the README file to see what you got"

clean:
	rm -f $(CLEANFILES)

distclean:
	rm -f $(CLEANFILES) config.cache config.log config.status config.h Make.defines
