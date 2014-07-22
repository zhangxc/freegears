/* conf.c
 *
 * Configuration file sample:
 * [global]
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
#include "wrapper.h"
#include "config.h"
#include "strbuf.h"

#include <linux/limits.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <regex.h>

static char *key;
static int do_all;
static int seen;
static char delim = '=';
static char term = '\n';

static FILE *config_file;
static const char *config_file_name;
static int config_linenr;
static int config_file_eof;
static const char *config_exclusive_filename = NULL;
//static int actions, types;


static int parse_unit_factor(const char *end, unsigned long *val)
{
	if (!*end)
		return 1;
	else if (!strcasecmp(end, "k")) {
		*val *= 1024;
		return 1;
	}
	else if (!strcasecmp(end, "m")) {
		*val *= 1024 * 1024;
		return 1;
	}
	else if (!strcasecmp(end, "g")) {
		*val *= 1024 * 1024 * 1024;
		return 1;
	}
	return 0;
}


static int fg_parse_long(const char *value, long *ret)
{
	if (value && *value) {
		char *end;
		long val = strtol(value, &end, 0);
		unsigned long factor = 1;
		if (!parse_unit_factor(end, &factor))
			return 0;
		*ret = val * factor;
		return 1;
	}
	return 0;
}


static void die_bad_config(const char *name)
{
	if (config_file_name)
		die("bad config value for '%s' in %s", name, config_file_name);
	die("bad config value for '%s'", name);
}



static int fg_config_maybe_bool_text(const char *name, const char *value)
{
	if (!value)
		return 1;
	if (!*value)
		return 0;
	if (!strcasecmp(value, "true")
	    || !strcasecmp(value, "yes")
	    || !strcasecmp(value, "on"))
		return 1;
	if (!strcasecmp(value, "false")
	    || !strcasecmp(value, "no")
	    || !strcasecmp(value, "off"))
		return 0;
	return -1;
}


static int config_error_nonbool(const char *var)
{
	return error("Missing value for '%s'", var);
}


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

unsigned long fg_config_ulong(const char *name, const char *value)
{
	unsigned long ret = 0;

	return ret;
}

int fg_config_int(const char *name, const char *value)
{
	long ret = 0;

	if (!fg_parse_long(value, &ret))
		die_bad_config(name);
	return ret;
}

static int fg_config_bool_or_int(const char *name, const char *value, int *is_bool)
{
	int v = fg_config_maybe_bool_text(name, value);
	if (0 <= v) {
		*is_bool = 1;
		return v;
	}
	*is_bool = 0;
	return fg_config_int(name, value);
}

int fg_config_maybe_bool(const char *name, const char *value)
{
	long v = fg_config_maybe_bool_text(name, value);
	if (0 <= v)
		return v;
	if (fg_parse_long(value, &v))
		return !!v;
	return -1;
}


int fg_config_bool(const char *name, const char *value)
{
	int discard;

	return !!fg_config_bool_or_int(name, value, &discard);
}


int fg_config_string(const char **dest, const char *var, const char *value)
{
	if (!value) {
		return config_error_nonbool(var);
	}
	*dest = xstrdup(value);

	return 0;
}

static int fg_config_parse_key(const char *key, char **store_key, int *baselen_)
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


static int fg_parse_file(config_fn_t fn, void *data)
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
	die("bad config file line %d in %s\n", config_linenr, config_file_name);
}


static int fg_config_from_file(config_fn_t fn, const char *filename, void *data)
{
	int ret;
	FILE *f = fopen(filename, "r");

	ret = -1;
	if (f) {
		config_file = f;
		config_file_name = filename;
		config_linenr = 1;
		config_file_eof = 0;
		ret = fg_parse_file(fn, data);
		fclose(f);
		config_file_name = NULL;
	}
	return ret;
}

/*
 * Find all the stuff for fg_config_set() below.
 */

#define MAX_MATCHES 512

static struct {
	int baselen;
	char *key;
	int do_not_match;
	regex_t *value_regex;
	int multi_replace;
	size_t offset[MAX_MATCHES];
	enum { START, SECTION_SEEN, SECTION_END_SEEN, KEY_SEEN } state;
	int seen;
} store;

static int matches(const char *key, const char *value)
{
	return !strcmp(key, store.key) &&
		(store.value_regex == NULL ||
		 (store.do_not_match ^
		  !regexec(store.value_regex, value, 0, NULL, 0)));
}


static int store_aux(const char *key, const char *value, void *cb)
{
	const char *ep;
	size_t section_len;

	switch (store.state) {
	case KEY_SEEN:
		if (matches(key, value)) {
			if (store.seen == 1 && store.multi_replace == 0) {
				warning("%s has multiple values", key);
			} else if (store.seen >= MAX_MATCHES) {
				error("too many matches for %s", key);
				return 1;
			}

			store.offset[store.seen] = ftell(config_file);
			store.seen++;
		}
		break;
	case SECTION_SEEN:
		/*
		 * What we are looking for is in store.key (both
		 * section and var), and its section part is baselen
		 * long.  We found key (again, both section and var).
		 * We would want to know if this key is in the same
		 * section as what we are looking for.  We already
		 * know we are in the same section as what should
		 * hold store.key.
		 */
		ep = strrchr(key, '.');
		section_len = ep - key;

		if ((section_len != store.baselen) ||
		    memcmp(key, store.key, section_len+1)) {
			store.state = SECTION_END_SEEN;
			break;
		}

		/*
		 * Do not increment matches: this is no match, but we
		 * just made sure we are in the desired section.
		 */
		store.offset[store.seen] = ftell(config_file);
		/* fallthru */
	case SECTION_END_SEEN:
	case START:
		if (matches(key, value)) {
			store.offset[store.seen] = ftell(config_file);
			store.state = KEY_SEEN;
			store.seen++;
		} else {
			if (strrchr(key, '.') - key == store.baselen &&
			      !strncmp(key, store.key, store.baselen)) {
					store.state = SECTION_SEEN;
					store.offset[store.seen] = ftell(config_file);
			}
		}
	}
	return 0;
}


static ssize_t find_beginning_of_line(const char *contents, size_t size,
	size_t offset_, int *found_bracket)
{
	size_t equal_offset = size, bracket_offset = size;
	ssize_t offset;

contline:
	for (offset = offset_-2; offset > 0
			&& contents[offset] != '\n'; offset--)
		switch (contents[offset]) {
			case '=': equal_offset = offset; break;
			case ']': bracket_offset = offset; break;
		}
	if (offset > 0 && contents[offset-1] == '\\') {
		offset_ = offset;
		goto contline;
	}
	if (bracket_offset < equal_offset) {
		*found_bracket = 1;
		offset = bracket_offset+1;
	} else
		offset++;

	return offset;
}


static int store_write_section(int fd, const char *key)
{
	const char *dot;
	int i, success;
	struct strbuf sb = STRBUF_INIT;

	dot = memchr(key, '.', store.baselen);
	if (dot) {
		strbuf_addf(&sb, "[%.*s \"", (int)(dot - key), key);
		for (i = dot - key + 1; i < store.baselen; i++) {
			if (key[i] == '"' || key[i] == '\\')
				strbuf_addch(&sb, '\\');
			strbuf_addch(&sb, key[i]);
		}
		strbuf_addstr(&sb, "\"]\n");
	} else {
		strbuf_addf(&sb, "[%.*s]\n", store.baselen, key);
	}

	success = write_in_full(fd, sb.buf, sb.len) == sb.len;
	strbuf_release(&sb);

	return success;
}

static int store_write_pair(int fd, const char *key, const char *value)
{
	int i, success;
	int length = strlen(key + store.baselen + 1);
	const char *quote = "";
	struct strbuf sb = STRBUF_INIT;

	/*
	 * Check to see if the value needs to be surrounded with a dq pair.
	 * Note that problematic characters are always backslash-quoted; this
	 * check is about not losing leading or trailing SP and strings that
	 * follow beginning-of-comment characters (i.e. ';' and '#') by the
	 * configuration parser.
	 */
	if (value[0] == ' ')
		quote = "\"";
	for (i = 0; value[i]; i++)
		if (value[i] == ';' || value[i] == '#')
			quote = "\"";
	if (i && value[i - 1] == ' ')
		quote = "\"";

	strbuf_addf(&sb, "\t%.*s = %s",
		    length, key + store.baselen + 1, quote);

	for (i = 0; value[i]; i++)
		switch (value[i]) {
		case '\n':
			strbuf_addstr(&sb, "\\n");
			break;
		case '\t':
			strbuf_addstr(&sb, "\\t");
			break;
		case '"':
		case '\\':
			strbuf_addch(&sb, '\\');
		default:
			strbuf_addch(&sb, value[i]);
			break;
		}
	strbuf_addf(&sb, "%s\n", quote);

	success = write_in_full(fd, sb.buf, sb.len) == sb.len;
	strbuf_release(&sb);

	return success;
}


struct lockfile {
	struct lockfile *next;
	int fd;
	pid_t owner;
	char on_list;
	char filename[PATH_MAX];
};


/*
 * If value==NULL, unset in (remove from) config,
 * if value_regex!=NULL, disregard key/value pairs where value does not match.
 * if multi_replace==0, nothing, or only one matching key/value is replaced,
 *     else all matching key/values (regardless how many) are removed,
 *     before the new pair is written.
 *
 * Returns 0 on success.
 *
 * This function does this:
 *
 * - it locks the config file by creating ".git/config.lock"
 *
 * - it then parses the config using store_aux() as validator to find
 *   the position on the key/value pair to replace. If it is to be unset,
 *   it must be found exactly once.
 *
 * - the config file is mmap()ed and the part before the match (if any) is
 *   written to the lock file, then the changed part and the rest.
 *
 * - the config file is removed and the lock file rename()d to it.
 *
 */
int fg_config_set_multivar(const char *key, const char *value,
	const char *value_regex, int multi_replace)
{
	int fd = -1, in_fd;
	int ret;
	char *config_filename;
	struct lockfile *lock = NULL;

	if (config_exclusive_filename)
		config_filename = xstrdup(config_exclusive_filename);
	else
		return -1;

	/* parse-key returns negative; flip the sign to feed exit(3) */
	ret = 0 - fg_config_parse_key(key, &store.key, &store.baselen);
	if (ret)
		goto out_free;

	store.multi_replace = multi_replace;

	/*
	 * The lock serves a purpose in addition to locking: the new
	 * contents of .git/config will be written into it.
	 */
	lock = xcalloc(sizeof(struct lockfile), 1);
	//fd = hold_lock_file_for_update(lock, config_filename, 0);
	if (strlen(config_filename) >= sizeof(lock->filename))
		return -1;
	//strcpy(lock->filename, config_filename);
	snprintf(lock->filename, PATH_MAX, "%s.lock", config_filename);
	lock->fd = open(lock->filename, O_RDWR | O_CREAT | O_EXCL, 0666);
	fd = lock->fd;
	if (fd < 0) {
		error("could not lock config file %s: %s", config_filename, strerror(errno));
		free(store.key);
		ret = -1;
		goto out_free;
	}

	in_fd = open(config_filename, O_RDONLY);
	if ( in_fd < 0 ) {
	/*
	 * If .git/config does not exist yet, write a minimal version.
	 */

	} else {
		struct stat st;
		char *contents;
		size_t contents_sz, copy_begin, copy_end;
		int i, new_line = 0;

		if (value_regex == NULL)
			store.value_regex = NULL;
/*		else {
			if (value_regex[0] == '!') {
				store.do_not_match = 1;
				value_regex++;
			} else
				store.do_not_match = 0;

			store.value_regex = (regex_t*)xmalloc(sizeof(regex_t));
			if (regcomp(store.value_regex, value_regex,
					REG_EXTENDED)) {
				error("invalid pattern: %s", value_regex);
				free(store.value_regex);
				ret = 6;
				goto out_free;
			}
		}
*/
		store.offset[0] = 0;
		store.state = START;
		store.seen = 0;

		/*
		 * After this, store.offset will contain the *end* offset
		 * of the last match, or remain at 0 if no match was found.
		 * As a side effect, we make sure to transform only a valid
		 * existing config file.
		 */
		if (fg_config_from_file(store_aux, config_filename, NULL)) {
			error("invalid config file %s", config_filename);
			free(store.key);
			if (store.value_regex != NULL) {
				regfree(store.value_regex);
				free(store.value_regex);
			}
			ret = 3;
			goto out_free;
		}

		free(store.key);
/*		if (store.value_regex != NULL) {
			regfree(store.value_regex);
			free(store.value_regex);
		}
*/
		/* if nothing to unset, or too many matches, error out */
		if ((store.seen == 0 && value == NULL) ||
				(store.seen > 1 && multi_replace == 0)) {
			ret = 5;
			goto out_free;
		}

		fstat(in_fd, &st);
		contents_sz = xsize_t(st.st_size);
		contents = mmap(NULL, contents_sz, PROT_READ,
			MAP_PRIVATE, in_fd, 0);
		if (!contents)
			die_errno("Out of memory? mmap failed");
		close(in_fd);

		if (store.seen == 0)
			store.seen = 1;

		for (i = 0, copy_begin = 0; i < store.seen; i++) {
			if (store.offset[i] == 0) {
				store.offset[i] = copy_end = contents_sz;
			} else if (store.state != KEY_SEEN) {
				copy_end = store.offset[i];
			} else
				copy_end = find_beginning_of_line(
					contents, contents_sz,
					store.offset[i]-2, &new_line);

			if (copy_end > 0 && contents[copy_end-1] != '\n')
				new_line = 1;

			/* write the first part of the config */
			if (copy_end > copy_begin) {
				if (write_in_full(fd, contents + copy_begin,
						  copy_end - copy_begin) <
				    copy_end - copy_begin)
					goto write_err_out;
				if (new_line &&
				    write_str_in_full(fd, "\n") != 1)
					goto write_err_out;
			}
			copy_begin = store.offset[i];
		}

		/* write the pair (value == NULL means unset) */
		if (value != NULL) {
			if (store.state == START) {
				if (!store_write_section(fd, key))
					goto write_err_out;
			}
			if (!store_write_pair(fd, key, value))
				goto write_err_out;
		}

		/* write the rest of the config */
		if (copy_begin < contents_sz)
			if (write_in_full(fd, contents + copy_begin,
					  contents_sz - copy_begin) <
			    contents_sz - copy_begin)
				goto write_err_out;

		munmap(contents, contents_sz);
	}


	if (lock->fd >= 0) {
		char result_file[PATH_MAX];
		int i;

		close(lock->fd);
		strcpy(result_file, lock->filename);
		i = strlen(result_file) - 5; /* .lock */
		result_file[i] = 0;
		if (rename(lock->filename, result_file))
			return -1;
		lock->filename[0] = 0;
	} else {
		ret = -1;
		goto out_free;
	}

/*	if (commit_lock_file(lock) < 0) {
		error("could not commit config file %s", config_filename);
		ret = 4;
		goto out_free;
	}
*/
	/*
	 * lock is committed, so don't try to roll it back below.
	 * NOTE: Since lockfile.c keeps a linked list of all created
	 * lock_file structures, it isn't safe to free(lock).  It's
	 * better to just leave it hanging around.
	 */
	lock = NULL;
	ret = 0;

out_free:
	if (lock) {
		if (lock->filename[0]) {
		if (lock->fd >= 0)
			close(lock->fd);
			unlink(lock->filename);
		}
		lock->filename[0] = 0;
	}
	free(config_filename);
	return ret;

write_err_out:
//	ret = write_error(lock->filename);
	goto out_free;

}


static int show_single_config(const char *key_, const char *value_, void *cb)
{
	if(!cb)
		return -1;

	if (!strcmp(key_, key)) {
		if (seen++ > 0) {
			warning("%s has multiple values, use the first one", key);
			return seen;
		}
		return fg_config_string((const char**)cb, key_, value_);
	}
	return 0;
}


int show_all_config(const char *key_, const char *value_, void *cb)
{
	if (value_)
		printf("%s%c%s%c", key_, delim, value_, term);
	else
		printf("%s%c", key_, term);
	return 0;
}


int fg_config(config_fn_t fn, void *data)
{
	const char *repo_config = NULL;
	int ret = 0;

	repo_config = config_exclusive_filename;

	if (repo_config && !access(repo_config, R_OK))
		ret = fg_config_from_file(fn, repo_config, data);

	return ret;
}

int fg_get_conf(const char* key_, char **value)
{
	int ret = 0;

	seen = 0;
	do_all = 0;
	if (fg_config_parse_key(key_, &key, NULL))
		goto free_strings;

	fg_config_from_file(show_single_config, config_exclusive_filename, value);

	seen = 0;

	free(key);

	if (do_all)
		ret = !seen;
	else
		ret = (seen == 1) ? 0 : seen > 1 ? 2 : 1;
free_strings:
	return ret;
}

int fg_set_conf(const char *key, const char *value)
{
	return fg_config_set_multivar(key, value, NULL, 0);
}

int init_fg_config(const char *filename)
{
	if (!filename)
		config_file_name = DEFAULT_CONF_FILE;
	else
		config_file_name = filename;

	config_exclusive_filename = config_file_name;

	return 0;
}
