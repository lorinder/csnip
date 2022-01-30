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

#include <csnip/csnip_conf.h>

#include <stddef.h>
#include <stdarg.h>

#if defined(CSNIP_CONF__HAVE_READV) || defined(CSNIP_CONF__HAVE_WRITEV)
#  include <sys/uio.h>
#endif

#if defined(CSNIP_CONF__HAVE_SYS_TYPES_H)
#  include <sys/types.h>
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


/**	Portable strerror_r().
 *
 *	This implements the POSIX conforming version of strerror_r().
 *	On Unix, the libc strerror_r() is used without.  There is no
 *	need in user code to define the feature macros that are needed
 *	to access the system strerror_r().  On Windows, strerror_s() is
 *	used in the backend.
 */
int csnip_x_strerror_r(int errnum, char* buf, size_t buflen);

/**	Portable asprintf().
 *
 *	csnip's asprintf() wrapper, known from GNU libc.  In the
 *	error case, -1 is returned and *strp is set to NULL just like
 *	the FreeBSD version of asprintf() does.
 */
int csnip_x_asprintf(char** strp, const char* format, ...);

/**	Portable vasprintf().
 *
 *	@sa csnip_x_asprintf().
 */
int csnip_x_vasprintf(char** strp, const char* format, va_list ap);

/**	Portable strdup(). */
char* csnip_x_strdup(const char* s);

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

/**	Portable writev().  */
csnip_x_ssize_t csnip_x_writev(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt);

/**	Portable readv(). */
csnip_x_ssize_t csnip_x_readv(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt);

/** @} */

#endif /* CSNIP_X_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_X_HAVE_SHORT_NAMES)
#define x_ssize_t			csnip_x_ssize_t
#define x_iovec				csnip_x_iovec
#define x_strerror_r			csnip_x_strerror_r
#define x_asprintf			csnip_x_asprintf
#define x_vasprintf			csnip_x_vasprintf
#define x_strdup			csnip_x_strdup
#define x_writev			csnip_x_writev
#define x_readv				csnip_x_readv
#define CSNIP_X_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_X_HAVE_SHORT_NAMES */
