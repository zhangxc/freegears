/* dlshaerd.cpp - dynamic link test
 *
 * suggest not to link this into your link library
 * original from http://rachid.koucha.free.fr/tech_corner/shared_libs_tests.html
 */
#include <stdio.h>
#include <iostream>

using namespace std;

class toto {
public:
	toto() { cout << "Constructor of toto\n"; var = 18; };
	~toto() { cout << "Destructor of toto\n"; var = 0; }
	int var;
};


toto glob_class;

extern "C" {
	void dl_initialize(void)
	{
		printf("Shared lib entry point, toto's var = %d\n", glob_class.var);
	}
}

