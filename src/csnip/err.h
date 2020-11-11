#ifndef CSNIP_ERR_H
#define CSNIP_ERR_H

/**	@file err.h
 *	@brief			Error handling
 *	@defgroup err		Error handling
 *	@{
 *
 *	@brief Error handling mechanism.
 *
 *	Csnip provides an error handling mechanism for its macro
 *	designed to be flexible and simple to use, but not get in the
 *	way.  Macros that have failure conditions have an error return
 *	containing either:
 *
 *	1.  The name of an "int" variable that is set to one of the
 *	    error codes if an error occurs.
 *
 *	2.  The special name "_" that results in the csnip_err_Unhandled
 *	    macro being invoked if an error occurs.  By default that
 *	    macro prints an error message to stderr and terminates the
 *	    program, but the user can define or redefine that macro to
 *	    do something else.
 *
 *	3.  The special name "error_ignore" that results in potential
 *	    error being ignored.  This is occasionally useful when it's
 *	    known that the possible errors are not critical, or that
 *	    they can be detected through other means than csnip's error
 *	    handling.
 *
 *	Csnip error handling is typically not used for user errors or
 *	direct validation of user input.
 */

#ifndef csnip_err_Unhandled
#include <stdio.h>
#include <stdlib.h>
/**	Handler for uncaught errors.
 *
 *	This macro is "invoked" when an error occurs while _ is used as
 *	the error return value.  You can redefine this to whatever you
 *	want rather than this default implementation.
 */
#define csnip_err_Unhandled(errnumber) \
	do { \
		char buf[128]; \
		csnip_err_str(errnumber, buf, sizeof(buf)); \
		fprintf(stderr, "%s:%d: %s\n", \
			__FILE__, \
			__LINE__, \
			buf); \
		_Exit(127); \
	} while(0)
#endif

/**	Raises an error in a macro.
 *
 *	err_expr is the error variable passed by the macro user.
 *	err_expr can be one of the following:
 *
 *	* An lvalue (lvalue expression not containing symbols _ or
 *	  error_ignore).  In this case the error code is stored in the
 *	  err_expr value.
 *
 *	* The special symbol _.  In this case the csnip_err_Unhandled
 *	  macro is invoked.  The default implementation of this macro
 *	  prints out an error message an terminates the application, but
 *	  the user can redefine it to do anything she wants.
 *
 *	* An lvalue dereferencing the NULL pointer (e.g. *(int*)0); this
 *	  is equivalent to _.
 *
 *	* The special symbol error_ignore.  The error is ignored, i.e.,
 *	  no error value is stored.  This is useful, for example, when
 *	  errors can be detected by means other than inspecting the error
 *	  variable.
 */
#define csnip_err_Raise(value, err_expr) \
	do { \
		int _, error_ignore; \
		(void)_; (void)error_ignore; \
		_Pragma("GCC diagnostic ignored \"-Waddress\""); \
		if (0 == &(err_expr) || &_ == &(err_expr)) { \
			csnip_err_Unhandled(value); \
		} \
		(err_expr) = (value); \
	} while(0)

/** @defgroup errCodes		Csnip error codes
 *  @{
 */
#define csnip_err_SUCCESS		0	/**< Success / no error */
#define csnip_err_ERRNO			(-1)	/**< Error code in errno */
#define csnip_err_NOMEM			(-2)	/**< Out of memory */
#define csnip_err_UNDERFLOW		(-3)	/**< Data structure underflow */
#define csnip_err_RANGE			(-4)	/**< Range error */
#define csnip_err_FORMAT		(-5)	/**< Format error */
#define csnip_err_UNEXPECTED_NULL	(-6)	/**< Invalid NULL pointer */
#define csnip_err_INVAL			(-7)	/**< Invalid value */
#define csnip_err_CALLFLOW		(-8)	/**< Invalid call flow */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/** Return a descriptive error string.
 *
 *  Given a csnip error code, write a descriptive string into a caller
 *  supplied buffer.
 *
 *  @param[in]	errnum
 *		the csnip error code (one of the csnip_err_* constants).
 *
 *  @param	buf
 *		the buffer to write the error to.
 *
 *  @param	buf_len
 *		the length of the supplied buffer.
 */
void csnip_err_str(int errnum, char* buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CSNIP_ERR_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_ERR_HAVE_SHORT_NAMES)
#define err_Unhandled		csnip_err_Unhandled
#define err_str			csnip_err_str
#define err_SUCCESS		csnip_err_SUCCESS
#define err_ERRNO		csnip_err_ERRNO
#define err_NOMEM		csnip_err_NOMEM
#define err_UNDERFLOW		csnip_err_UNDERFLOW
#define err_RANGE		csnip_err_RANGE
#define err_FORMAT		csnip_err_FORMAT
#define err_UNEXPECTED_NULL	csnip_err_UNEXPECTED_NULL
#define err_INVAL		csnip_err_INVAL
#define err_CALLFLOW		csnip_err_CALLFLOW
#define CSNIP_ERR_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_ERR_HAVE_SHORT_NAMES */
