/* dltest.cpp - explicit dynamic link test
 *
 * This program shows how to use the API of dynamic linking loader. 
 * See lib/dlshared.cpp for details.
 *
 * Original from http://rachid.koucha.free.fr/tech_corner/shared_libs_tests.html
 */

#include <stdio.h>
#include <dlfcn.h>

extern "C" { void dl_initialize(void); }

int main(void)
{
	void *hdl;
	void *sym;
	char *err;
	void (*call)(void);

	printf("Main entry point\n");

	// Check parameters
	printf("Loading shared lib...\n");
	hdl = dlopen("../lib/dlshared.so", RTLD_LAZY | RTLD_GLOBAL);
	if (NULL == hdl) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}

	// Clear any pending error message
	(void)dlerror();

	// Look for symbol in the shared lib
	sym = dlsym(hdl, "dl_initialize");
	if (NULL == sym) {
		err = dlerror();
		if (err) {
			fprintf(stderr, "%s\n", err);
			return 1;
		} else {
 			// The symbol has been found but it is NULL
 			fprintf(stderr, "The symbol is NULL\n");
 			return 1;
 	 	}
	}

	// Call a function in shared lib
	call = (void (*)(void))sym;
	(*call)();

	return 0;
}

