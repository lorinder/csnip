#ifndef CSNIP_X_H
#define CSNIP_X_H

/**	@file x.h
 *	@defgroup x		Compatibility functions
 *	@{
 *
 *	Portable implementations of libc functions.
 *
 *	This module contains functions that can be found on some
 *	systems, but are missing from others.  If the system has a
 *	native version of the API, that is generally used to implement
 *	csnip's wrapper.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <time.h>

#include <csnip/csnip_conf.h>

#if defined(CSNIP_CONF__HAVE_READV) || defined(CSNIP_CONF__HAVE_WRITEV)
#  include <sys/uio.h>
#endif
#if defined(CSNIP_CONF__HAVE_SYS_TYPES_H)
#  include <sys/types.h>
#endif
#if defined(CSNIP_CONF__HAVE_GETOPT)
#  include <unistd.h>
#endif

#include <csnip/cext.h>

/* Mark symbol for export.
 *
 * XXX this should not be specific to x.h; other modules could use
 * this just as well.  Currently they don't, however.
 */
#ifdef csnip_EXPORTS
#  define CSNIP_SYMBOL	csnip_cext_export
#else
#  define CSNIP_SYMBOL	csnip_cext_import
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**	ssize_t */
#if defined(CSNIP_CONF__HAVE_SSIZE_T)
#  define csnip_x_ssize_t ssize_t
#elif defined(CSNIP_CONF__HAVE_STDINT_H) && CHAR_BIT == 8
#  include <stdint.h>
#  if CSNIP_CONF__SIZEOF_SIZE_T == 8
     typedef int64_t csnip_x_ssize_t;
#  elif CSNIP_CONF__SIZEOF_SIZE_T == 4
     typedef int32_t csnip_x_ssize_t;
#  elif CSNIP_CONF__SIZEOF_SIZE_T == 2
     typedef int16_t csnip_x_ssize_t;
#  else
#    error "Unsupported sizeof(size_t)"
#  endif
#else
#  error "Can't determine a suitable type for csnip_x_ssize_t"
#endif


/**	Wrapper for asprintf() or csnip_x_asprintf_imp().  */
#define csnip_x_asprintf asprintf
#ifndef CSNIP_CONF__HAVE_ASPRINTF
#undef csnip_x_asprintf
#define csnip_x_asprintf csnip_x_asprintf_imp
#else
int asprintf(char** strp, const char* fmt, ...);
#endif

/**	Portable asprintf().  */
int csnip_x_asprintf_imp(char** strp, const char* format, ...);

/**	Wrapper for getdelim or csnip_x_getdelim_imp() */
#define csnip_x_getdelim getdelim
#if !defined(CSNIP_CONF__HAVE_GETDELIM)
#undef csnip_x_getdelim
#define csnip_x_getdelim csnip_x_getdelim_imp
#endif

/**	Csnip's own getdelim() */
csnip_x_ssize_t csnip_x_getdelim_imp(char** lineptr,
			size_t* n,
			int delim,
			FILE* fp);

/**	Wrapper for getline or csnip_x_getline_imp() */
#define csnip_x_getline getline
#if !defined(CSNIP_CONF__HAVE_GETLINE)
#undef csnip_x_getline
#define csnip_x_getline csnip_x_getline_imp
#endif

/**	Csnip's own getline() */
csnip_x_ssize_t csnip_x_getline_imp(char** lineptr,
			size_t* n,
			FILE* fp);

/**	Wrapper for getopt or csnip_x_getopt_imp() */
#define csnip_x_getopt getopt
#if !defined(CSNIP_CONF__HAVE_GETOPT)
#undef csnip_x_getopt
#define csnip_x_getopt csnip_x_getopt_imp
#endif

/**	Csnip's own getopt() */
int csnip_x_getopt_imp(int argc, char* argv[], const char* optstring);

/**	Portable struct iovec.
 *
 *	When the system has it, we just alias our iovec to the system's.
 *	Otherwise we roll our own.
 */
#if defined(CSNIP_CONF__HAVE_READV) || defined(CSNIP_CONF__HAVE_WRITEV)
#  define csnip_x_iovec	iovec
#else
struct csnip_x_iovec {
	void  *iov_base;
	size_t iov_len;
};
#endif

/**	optarg */
#define csnip_x_optarg optarg
#if !defined(CSNIP_CONF__HAVE_GETOPT)
#undef csnip_x_optarg
#define csnip_x_optarg csnip_x_optarg_imp
#endif
CSNIP_SYMBOL extern char* csnip_x_optarg_imp;

/**	opterr */
#define csnip_x_opterr opterr
#if !defined(CSNIP_CONF__HAVE_GETOPT)
#undef csnip_x_opterr
#define csnip_x_opterr csnip_x_opterr_imp
#endif
CSNIP_SYMBOL extern int csnip_x_opterr_imp;

/**	optind */
#define csnip_x_optind optind
#if !defined(CSNIP_CONF__HAVE_GETOPT)
#undef csnip_x_optind
#define csnip_x_optind csnip_x_optind_imp
#endif
CSNIP_SYMBOL extern int csnip_x_optind_imp;

/**	optopt */
#define csnip_x_optopt optopt
#if !defined(CSNIP_CONF__HAVE_GETOPT)
#undef csnip_x_optopt
#define csnip_x_optopt csnip_x_optopt_imp
#endif
CSNIP_SYMBOL extern int csnip_x_optopt_imp;

/**	Wrapper for readv() or csnip_x_readv_imp().
 *
 *	Maps to readv() on systems that have it, and to
 *	csnip_x_readv_imp() otherwise.
 */
#define csnip_x_readv readv
#if !defined(CSNIP_CONF__HAVE_READV)
#undef csnip_x_readv
#define csnip_x_readv csnip_x_readv_imp
#endif

/**	Csnip's own readv().
 *
 *	Reads into a temporary buffer using read(), then copies into the
 *	scattered blocks.
 */
csnip_x_ssize_t csnip_x_readv_imp(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt);

/**	Portable strdup(). */
char* csnip_x_strdup(const char* s);

/**	Portable strerror_r().
 *
 *	This implements the POSIX conforming version of strerror_r().
 *	On Unix, the libc strerror_r() is used without.  There is no
 *	need in user code to define the feature macros that are needed
 *	to access the system strerror_r().  On Windows, strerror_s() is
 *	used in the backend.
 */
int csnip_x_strerror_r(int errnum, char* buf, size_t buflen);

/**	Wrapper for strtok_r() or csnip_x_strtok_r() */
#define csnip_x_strtok_r strtok_r
#if !defined(CSNIP_CONF__HAVE_STRTOK_R)
#undef csnip_x_strtok_r
#define csnip_x_strtok_r csnip_x_strtok_r_imp
#endif

/**	Csnip's own strtok_r() */
char* csnip_x_strtok_r_imp(char* str, const char* delim, char** saveptr);

/**	Wrapper for vasprintf() or csnip_x_vasprintf() */
#define csnip_x_vasprintf vasprintf
#ifndef CSNIP_CONF__HAVE_VASPRINTF
#undef csnip_x_vasprintf
#define csnip_x_vasprintf csnip_x_vasprintf_imp
#else
int vasprintf(char** strp, const char* fmt, va_list ap);
#endif

/**	Portable vasprintf().
 *
 *	@sa csnip_x_asprintf_imp().
 */
int csnip_x_vasprintf_imp(char** strp, const char* format, va_list ap);

/**	Wrapper for writev() or csnip_x_writev().
 *
 *	On systems that have writev(), that system call is used.
 *	Otherwise, the less efficient csnip_x_writev_imp() is used.
 */
#define csnip_x_writev writev
#if !defined(CSNIP_CONF__HAVE_WRITEV)
#undef csnip_x_writev
#define csnip_x_writev csnip_x_writev_imp
#endif

/**	Csnip's own writev().
 *
 *	Behaves like writev(), though isn't as efficient.
 *
 *	This implementation first copies the scattered blocks into a
 *	temporary buffer and then calls write().  If the write() system
 *	call respects POSIX' atomicity guarantees, then so will this
 *	implementation of readv().
 */
csnip_x_ssize_t csnip_x_writev_imp(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt);

/** @} */

/**	Wrapper for clock_gettime() or csnip_x_clock_gettime() */
#define csnip_x_clock_gettime clock_gettime
#if !defined(CSNIP_CONF__HAVE_CLOCK_GETTIME)
#undef csnip_x_clock_gettime
#define csnip_x_clock_gettime csnip_x_clock_gettime_imp
#endif

/**	Csnip's  CLOCK_* constants.
 *
 */
#if defined(CLOCK_REALTIME)
#  define CSNIP_X_CLOCK_REALTIME		CLOCK_REALTIME
#  ifdef CLOCK_MONOTONIC
#    define CSNIP_X_CLOCK_MONOTONIC		CLOCK_MONOTONIC
#    define CSNIP_X_CLOCK_MAYBE_MONOTONIC	CLOCK_MONOTONIC
#  else
#    define CSNIP_X_CLOCK_MAYBE_MONOTONIC	CLOCK_REALTIME
#  endif
#else
#  define CSNIP_X_CLOCK_REALTIME		1
#  define CSNIP_X_CLOCK_MAYBE_MONOTONIC		1
#endif

/**	clock_id_t */
#define csnip_x_clockid_t clockid_t
#if !defined(CSNIP_CONF__HAVE_CLOCK_GETTIME)
#  undef csnip_x_clockid_t
#  define csnip_x_clockid_t			int
#endif

/**	struct timespec */
#define csnip_x_timespec timespec
#if !defined(CSNIP_CONF__HAVE_TIMESPEC)
#  undef csnip_x_timespec
struct csnip_x_timespec {
	time_t tv_sec;
	long tv_nsec;
};
#endif

/**	Csnip's own clock_gettime() */
int x_csnip_clock_gettime_imp(csnip_x_clockid_t clk_id,
			struct csnip_x_timespec* ts);

#ifdef __cplusplus
}
#endif

#endif /* CSNIP_X_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_X_HAVE_SHORT_NAMES)

/* types */
#define x_ssize_t			csnip_x_ssize_t

/* API calls */
#define x_asprintf			csnip_x_asprintf
#define x_asprintf_imp			csnip_x_asprintf_imp
#define x_clock_gettime			csnip_x_clock_gettime
#define x_clock_gettime_imp		csnip_x_clock_gettime_imp
#define x_clock_id			csnip_x_clock_id
#define X_CLOCK_MAYBE_MONOTONIC		CSNIP_X_CLOCK_MAYBE_MONOTONIC
#ifdef CSNIP_X_CLOCK_MONOTONIC
#define X_CLOCK_MONOTONIC		CSNIP_X_CLOCK_MONOTONIC
#endif
#define X_CLOCK_REALTIME		CSNIP_X_CLOCK_REALTIME
#define x_getdelim			csnip_x_getdelim
#define x_getdelim_imp			csnip_x_getdelim_imp
#define x_getline			csnip_x_getline
#define x_getline_imp			csnip_x_getline_imp
#define x_getopt			csnip_x_getopt
#define x_getopt_imp			csnip_x_getopt_imp
#define x_iovec				csnip_x_iovec
#define x_optarg			csnip_x_optarg
#define x_optarg_imp			csnip_x_optarg_imp
#define x_opterr			csnip_x_opterr
#define x_opterr_imp			csnip_x_opterr_imp
#define x_optind			csnip_x_optind
#define x_optind_imp			csnip_x_optind_imp
#define x_optopt			csnip_x_optopt
#define x_optopt_imp			csnip_x_optopt_imp
#define x_readv				csnip_x_readv
#define x_readv_imp			csnip_x_readv_imp
#define x_strdup			csnip_x_strdup
#define x_strerror_r			csnip_x_strerror_r
#define x_strtok_r			csnip_x_strtok_r
#define x_strtok_r_imp			csnip_x_strtok_r_imp
#define x_timespec			csnip_x_timespec
#define x_vasprintf			csnip_x_vasprintf
#define x_vasprintf_imp			csnip_x_vasprintf_imp
#define x_writev			csnip_x_writev
#define x_writev_imp			csnip_x_writev_imp
#define CSNIP_X_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_X_HAVE_SHORT_NAMES */
