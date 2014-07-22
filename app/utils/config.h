#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "usage.h"

#define DEFAULT_CONF_FILE "user.conf"
#define MAXNAME (256)

#define TYPE_BOOL (1<<0)
#define TYPE_INT (1<<1)
#define TYPE_BOOL_OR_INT (1<<2)
#define TYPE_PATH (1<<3)

#define ACTION_GET (1<<0)
#define ACTION_GET_ALL (1<<1)
#define ACTION_GET_REGEXP (1<<2)
#define ACTION_REPLACE_ALL (1<<3)
#define ACTION_LIST (1<<9)
#define ACTION_EDIT (1<<10)
#define ACTION_SET (1<<11)
#define ACTION_SET_ALL (1<<12)

static inline size_t xsize_t(off_t len)
{
	if (len > (size_t) len)
		die("Cannot handle files this big");
	return (size_t)len;
}

typedef int (*config_fn_t)(const char *, const char *, void *);
extern int get_conf_generic(const char *var, const char *value, void *cb);
extern int fg_config_int(const char *name, const char *value);
extern unsigned long fg_config_ulong(const char *name, const char *value);
extern int fg_config_bool(const char *name, const char *value);
extern int fg_config_string(const char **dest, const char *var, const char *value);
extern int fg_config(config_fn_t fn, void *data);
extern int show_all_config(const char *key_, const char *value_, void *cb);
extern int init_fg_config(const char *filename);
int fg_get_conf(const char* key_, char **value);
int fg_set_conf(const char *key, const char *value);


#endif
