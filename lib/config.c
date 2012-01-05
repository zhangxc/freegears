/* conf.c
 * 
 * Configuration file sample:
 *   editor = vim
 *   prompt = false
 * [user]
 *   name = Snake
 *   email = i@g.cn
 * 
 * Transforming the configration into variables:
 *   core.editor = vim
 *   core.prompt = false
 *   user.name = Snake
 *   user.email = i@g.cn
 */
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#define FDEBUG
#ifdef FDEBUG
#  define DEBUG(fmt, args...) printf(fmt, ##args)
#else
#  define DEBUG(fmt, args...)
#endif

#define CONF_FILENAME "user.conf"
#define MAXNAME (256)
#define TYPE_BOOL (1<<0)
#define TYPE_INT (1<<1)
#define TYPE_BOOL_OR_INT (1<<2)
#define TYPE_PATH (1<<3)

typedef int (*config_fn_t)(const char *, const char *, void *);

static char *key;
static int show_keys;
static int do_all;
static int seen;
static char delim = '=';
static char key_delim = ' ';
static char term = '\n';

static FILE *config_file;
static const char *config_file_name;
static int config_linenr;
static int config_file_eof;
const char *config_exclusive_filename = NULL;
static int actions; //, types;

static int get_next_char(void)
{
	int c;
	FILE *f;

	c = '\n';
	if ((f = config_file) != NULL) {
		c = fgetc(f);
		if (c == '\r') {
			/* DOS like systems */
			c = fgetc(f);
			if (c != '\n') {
				ungetc(c, f);
				c = '\r';
			}
		}
		if (c == '\n')
			config_linenr++;
		if (c == EOF) {
			config_file_eof = 1;
			c = '\n';
		}
	}
	return c;
}

static char *parse_value(void)
{
	static char value[1024];
	int quote = 0, comment = 0, len = 0, space = 0;

	for (;;) {
		int c = get_next_char();
		if (len >= sizeof(value) - 1)
			return NULL;
		if (c == '\n') {
			if (quote)
				return NULL;
			value[len] = 0;
			return value;
		}
		if (comment)
			continue;
		if (isspace(c) && !quote) {
			if (len)
				space++;
			continue;
		}
		if (!quote) {
			if (c == ';' || c == '#') {
				comment = 1;
				continue;
			}
		}
		for (; space; space--)
			value[len++] = ' ';
		if (c == '\\') {
			c = get_next_char();
			switch (c) {
			case '\n':
				continue;
			case 't':
				c = '\t';
				break;
			case 'b':
				c = '\b';
				break;
			case 'n':
				c = '\n';
				break;
			/* Some characters escape as themselves */
			case '\\': case '"':
				break;
			/* Reject unknown escape sequences */
			default:
				return NULL;
			}
			value[len++] = c;
			continue;
		}
		if (c == '"') {
			quote = 1-quote;
			continue;
		}
		value[len++] = c;
	}
}

static inline int iskeychar(int c)
{
	return isalnum(c) || c == '-';
}

int git_config_parse_key(const char *key, char **store_key, int *baselen_)
{
	int i, dot, baselen;
	const char *last_dot = strrchr(key, '.');

	/*
	 * Since "key" actually contains the section name and the real
	 * key name separated by a dot, we have to know where the dot is.
	 */

	if (last_dot == NULL || last_dot == key) {
		printf("key does not contain a section: %s\n", key);
		return -2;
	}

	if (!last_dot[1]) {
		printf("key does not contain variable name: %s\n", key);
		return -2;
	}

	baselen = last_dot - key;
	if (baselen_)
		*baselen_ = baselen;

	/*
	 * Validate the key and while at it, lower case it for matching.
	 */
	*store_key = malloc(strlen(key) + 1);

	dot = 0;
	for (i = 0; key[i]; i++) {
		unsigned char c = key[i];
		if (c == '.')
			dot = 1;
		/* Leave the extended basename untouched.. */
		if (!dot || i > baselen) {
			if (!iskeychar(c) ||
			    (i == baselen + 1 && !isalpha(c))) {
				printf("invalid key: %s\n", key);
				goto out_free_ret_1;
			}
			c = tolower(c);
		} else if (c == '\n') {
			printf("invalid key (newline): %s\n", key);
			goto out_free_ret_1;
		}
		(*store_key)[i] = c;
	}
	(*store_key)[i] = 0;

	return 0;

out_free_ret_1:
	free(*store_key);
	return -1;
}

static int get_extended_base_var(char *name, int baselen, int c)
{
	do {
		if (c == '\n')
			return -1;
		c = get_next_char();
	} while (isspace(c));

	/* We require the format to be '[base "extension"]' */
	if (c != '"')
		return -1;
	name[baselen++] = '.';

	for (;;) {
		int c = get_next_char();
		if (c == '\n')
			return -1;
		if (c == '"')
			break;
		if (c == '\\') {
			c = get_next_char();
			if (c == '\n')
				return -1;
		}
		name[baselen++] = c;
		if (baselen > MAXNAME / 2)
			return -1;
	}

	/* Final ']' */
	if (get_next_char() != ']')
		return -1;
	return baselen;
}


static int get_base_var(char *name)
{
	int baselen = 0;

	for (;;) {
		int c = get_next_char();
		if (config_file_eof)
			return -1;
		if (c == ']')
			return baselen;
		if (isspace(c))
			return get_extended_base_var(name, baselen, c);
		if (!iskeychar(c) && c != '.')
			return -1;
		if (baselen > MAXNAME / 2)
			return -1;
		name[baselen++] = tolower(c);
	}
}

static int get_value_2(config_fn_t fn, void *data, char *name, unsigned int len)
{
	int c;
	char *value;

	/* Get the full name */
	for (;;) {
		c = get_next_char();
		if (config_file_eof)
			break;
		if (!iskeychar(c))
			break;
		name[len++] = tolower(c);
		if (len >= MAXNAME)
			return -1;
	}
	name[len] = 0;
	while (c == ' ' || c == '\t')
		c = get_next_char();

	value = NULL;
	if (c != '\n') {
		if (c != '=')
			return -1;
		value = parse_value();
		if (!value)
			return -1;
	}
	return fn(name, value, data);
}


static int git_parse_file(config_fn_t fn, void *data)
{
	int comment = 0;
	int baselen = 0;
	static char var[MAXNAME];

	/* U+FEFF Byte Order Mark in UTF8 */
	static const unsigned char *utf8_bom = (unsigned char *) "\xef\xbb\xbf";
	const unsigned char *bomptr = utf8_bom;

	for (;;) {
		int c = get_next_char();
		if (bomptr && *bomptr) {
			/* We are at the file beginning; skip UTF8-encoded BOM
			 * if present. Sane editors won't put this in on their
			 * own, but e.g. Windows Notepad will do it happily. */
			if ((unsigned char) c == *bomptr) {
				bomptr++;
				continue;
			} else {
				/* Do not tolerate partial BOM. */
				if (bomptr != utf8_bom)
					break;
				/* No BOM at file beginning. Cool. */
				bomptr = NULL;
			}
		}
		if (c == '\n') {
			if (config_file_eof)
				return 0;
			comment = 0;
			continue;
		}
		if (comment || isspace(c))
			continue;
		if (c == '#' || c == ';') {
			comment = 1;
			continue;
		}
		if (c == '[') {
			baselen = get_base_var(var);
			if (baselen <= 0)
				break;
			var[baselen++] = '.';
			var[baselen] = 0;
			continue;
		}
		if (!isalpha(c))
			break;
		var[baselen] = tolower(c);
		if (get_value_2(fn, data, var, baselen+1) < 0)
			break;
	}
	printf("bad config file line %d in %s\n", config_linenr, config_file_name);
}


int git_config_from_file(config_fn_t fn, const char *filename, void *data)
{
	int ret;
	FILE *f = fopen(filename, "r");

	ret = -1;
	if (f) {
		config_file = f;
		config_file_name = filename;
		config_linenr = 1;
		config_file_eof = 0;
		ret = git_parse_file(fn, data);
		fclose(f);
		config_file_name = NULL;
	}
	return ret;
}

static int show_config(const char *key_, const char *value_, void *cb)
{
	char value[256];
	const char *vptr = value;
	int must_free_vptr = 0;
	int dup_error = 0;

	if (strcmp(key_, key))
		return 0;

	if (show_keys) {
		if (value_)
			printf("%s%c", key_, key_delim);
		else
			printf("%s", key_);
	}
	if (seen && !do_all)
		dup_error = 1;

/*	if (types == TYPE_INT)
		sprintf(value, "%d", git_config_int(key_, value_?value_:""));
	else if (types == TYPE_BOOL)
		vptr = git_config_bool(key_, value_) ? "true" : "false";
	else if (types == TYPE_BOOL_OR_INT) {
		int is_bool, v;
		v = git_config_bool_or_int(key_, value_, &is_bool);
		if (is_bool)
			vptr = v ? "true" : "false";
		else
			sprintf(value, "%d", v);
	} else if (types == TYPE_PATH) {
		git_config_pathname(&vptr, key_, value_);
		must_free_vptr = 1;
	}
	else */
		vptr = value_?value_:"";
	seen++;
	if (dup_error) {
		printf("More than one value for the key %s: %s\n",
				key_, vptr);
	}
	else
		printf("%s%c", vptr, term);
	if (must_free_vptr)
		/* If vptr must be freed, it's a pointer to a
		 * dynamically allocated buffer, it's safe to cast to
		 * const.
		*/
		free((char *)vptr);

	return 0;
}

static int get_value(const char *key_)
{
	int ret = -1;
	const char *local;

	local = config_exclusive_filename;
	if (!local)
		return -1;

	if (git_config_parse_key(key_, &key, NULL))
			goto free_strings;
	key = strdup(key_);

	git_config_from_file(show_config, local, NULL);

	free(key);

	if (do_all)
		ret = !seen;
	else
		ret = (seen == 1) ? 0 : seen > 1 ? 2 : 1;

free_strings:
	return ret;
}


int git_config_early(config_fn_t fn, void *data, const char *repo_config)
{
	int ret = 0, found = 0;

	if (repo_config && !access(repo_config, R_OK)) {
		ret += git_config_from_file(fn, repo_config, data);
		found += 1;
	}

/*	switch (git_config_from_parameters(fn, data)) {
	case -1: // error
		ret--;
		break;
	case 0: // found nothing
		break;
	default: // found at least one item
		found++;
		break;
	}
*/
	return ret == 0 ? found : ret;
}

int git_config(config_fn_t fn, void *data)
{
	char *repo_config = NULL;
	int ret;

	repo_config = config_exclusive_filename;
	ret = git_config_early(fn, data, repo_config);

	return ret;
}

static int show_all_config(const char *key_, const char *value_, void *cb)
{
	if (value_)
		printf("%s%c%s%c", key_, delim, value_, term);
	else
		printf("%s%c", key_, term);
	return 0;
}

int main(int argc, char *argv[])
{

	/* initialization */
	actions = ACTION_LIST;
	config_exclusive_filename = CONF_FILENAME;
	config_file_name = CONF_FILENAME;

	if (actions == ACTION_GET && argc != 2 ) {
		printf("%s must take one argument\n", argv[0]);
		exit(2);
	}

	switch(actions) {
	case ACTION_GET:
		get_value(argv[1]);
		break;
	case ACTION_LIST:
		git_config(show_all_config, NULL);
		break;
	case ACTION_SET:
		//value = normalize_value(argv[0], argv[1]);
		//git_config_set(argv[0], value)
		break;
	default:
		break;
	}

	return 0;
}
