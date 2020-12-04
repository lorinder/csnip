#ifndef CSNIP_CEXT_H
#define CSNIP_CEXT_H

/**	@file cext.h
 *	@brief			C Extension Macros
 *	@defgroup cext		C Extension Macros
 *	@{
 *
 *	@brief Macros for C extensions.
 *
 *	This provides a number of macros for extensions to C that are
 *	commonly available in C environments, but done differently in
 *	different environments.
 *
 *	Right now, only implementations for GCC/clang are included, but
 *	the idea is that other compilers can be easily added here, if
 *	they support the relevant features.
 */

/**	Mark a variable or function as potentially unused.
 *
 *	This silences possible compiler warnings related to unused
 *	functions.
 */
#if defined(__GNUC__) || defined(__clang__)
#define csnip_cext_unused		__attribute__((__unused__))
#else
#define csnip_cext_unused
#endif

/**	Mark a symbol as belonging to the public API.
 *
 *	This marks a symbol as being part of the public interface of a
 *	dynamically linked library.  If used with GCC or clang, you
 *	should use the -fvisibility=hidden flag to avoid marking all
 *	symbols, and then only selectively "export" such symbols.
 */
#if defined(__GNUC__) || defined(__clang__)
#define csnip_cext_export		__attribute__ ((visibility ("default")))
#endif

/**	Mark a symbol as being imported. */
#if defined(__GNUC__) || defined(__clang__)
#define csnip_cext_import
#endif

/**@}*/

#endif /* CSNIP_CEXT_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_CEXT_HAVE_SHORT_NAMES)
#define cext_unused		csnip_cext_unused
#define cext_export		csnip_cext_export
#define cext_import		csnip_cext_import
#define CSNIP_CEXT_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_CEXT_HAVE_SHORT_NAMES */
