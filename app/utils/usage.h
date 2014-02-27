/* usage.h
 */
#include <stdio.h>

#if __HP_cc >= 61000
#define NORETURN __attribute__((noreturn))
#define NORETURN_PTR
#elif defined(__GNUC__)
#define NORETURN __attribute__((__noreturn__))
#define NORETURN_PTR __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#define NORETURN_PTR
#else
#define NORETURN
#define NORETURN_PTR
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

/* General helper functions */
extern void vreportf(const char *prefix, const char *err, va_list params);
extern NORETURN void usage(const char *err);
extern NORETURN void usagef(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern NORETURN void die(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern NORETURN void die_errno(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern int error(const char *err, ...) __attribute__((format (printf, 1, 2)));
extern void warning(const char *err, ...) __attribute__((format (printf, 1, 2)));

extern void set_die_routine(NORETURN_PTR void (*routine)(const char *err, va_list params));


