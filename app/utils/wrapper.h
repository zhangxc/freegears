/* wrapper.h
 *
 */
#include "utils.h"

#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#define S_IFGITLINK	0160000
#define S_ISGITLINK(m)	(((m) & S_IFMT) == S_IFGITLINK)

typedef void (*try_to_free_t) (size_t);
extern try_to_free_t set_try_to_free_routine(try_to_free_t);

extern char *xstrdup(const char *str);
extern void *xmalloc(size_t size);
extern void *xmallocz(size_t size);
extern void *xmemdupz(const void *data, size_t len);
extern char *xstrndup(const char *str, size_t len);
extern ssize_t xstrlcpy(char *dst, const char *src, ssize_t size);
extern void *xrealloc(void *ptr, size_t size);
extern void *xcalloc(size_t nmemb, size_t size);
//extern void *xmmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
extern ssize_t xread(int fd, void *buf, size_t len);
extern ssize_t xwrite(int fd, const void *buf, size_t len);
extern ssize_t read_in_full(int fd, void *buf, size_t count);
extern ssize_t write_in_full(int fd, const void *buf, size_t count);

static inline ssize_t write_str_in_full(int fd, const char *str)
{
	return write_in_full(fd, str, strlen(str));
}

extern int xdup(int fd);
extern FILE *xfdopen(int fd, const char *mode);
extern int xmkstemp(char *template);
extern int xmkstemp_mode(char *template, int mode);
