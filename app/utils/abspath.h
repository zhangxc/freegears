/* abspath.h
 */

#ifndef is_dir_sep
#define is_dir_sep(c) ((c) == '/')
#endif

#ifndef has_dos_drive_prefix
#define has_dos_drive_prefix(path) 0
#endif

static inline int is_absolute_path(const char *path)
{
	return path[0] == '/' || has_dos_drive_prefix(path);
}

extern int is_directory(const char *);
extern const char *real_path(const char *path);
extern const char *absolute_path(const char *path);


