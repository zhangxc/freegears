#include "config.h"

int main(void)
{
	char *value;

	/* init the config library */
	init_fg_config("/home/wick/.gitconfig");

	/* API: fg_get_conf
	 *   @key
	 *   @value
	 *
	 *  CAUTION:
	 *    Take care of value-result argument @value, and remember to free
	 *    it before you call other malloc functions again.
	 */
	fg_get_conf("user.name", &value);
	if (value) {
		printf("%s\n", value);
		free(value);
	}
	return 0;
}
