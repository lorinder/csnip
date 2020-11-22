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

#include <stddef.h>
#include <stdarg.h>

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

/** @} */

#endif /* CSNIP_X_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_X_HAVE_SHORT_NAMES)
#define x_strerror_r			csnip_x_strerror_r
#define x_asprintf			csnip_x_asprintf
#define x_vasprintf			csnip_x_vasprintf
#define CSNIP_X_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_X_HAVE_SHORT_NAMES */
