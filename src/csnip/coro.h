#ifndef CSNIP_CORO_H
#define CSNIP_CORO_H

/**	@file coro.h
 *	@brief				Coroutines
 *	@defgroup coro			Coroutines
 *	@{
 *
 *	@brief Coroutines API
 *
 *	This module provides support for asymmetric coroutines.  Values
 *	can be passed both to and from the coroutine at each next/yield
 *	pair, maling it trivial to implement iterators and generators.
 *	Integration with event loops, such as e.g. provided with
 *	libevent, is also straightforward.
 *
 *	*Requirements*:  Currently requires System V context control API.
 *
 *	The following example code implements a coroutine, get_primes()
 *	that computes the sequence of prime numbers and passes them via
 *	csnip_coro_yield() to the main routine, which then prints them
 *	out.
 *
 *	\include coro.c
 */

/* Select the adequate implementation */
/** @cond */
#include <csnip/csnip_conf.h>

#if defined CSNIP_CORO_IS_UCTX
#include <csnip/coro_uctx.h>
#define csnip_coro_s		csnip_coro_uctx_s
#define csnip_coro		csnip_coro_uctx
#define csnip_coro_func_ptr	csnip_coro_uctx_func_ptr
#define csnip_coro_new		csnip_coro_uctx_new
#define csnip_coro_set_func	csnip_coro_uctx_set_func
#define csnip_coro_next		csnip_coro_uctx_next
#define csnip_coro_yield	csnip_coro_uctx_yield
#define csnip_coro_get_value	csnip_coro_uctx_get_value
#define csnip_coro_free		csnip_coro_uctx_free
#elif defined CSNIP_CORO_IS_PTH
#include <csnip/coro_pth.h>
#define csnip_coro_s		csnip_coro_pth_s
#define csnip_coro		csnip_coro_pth
#define csnip_coro_func_ptr	csnip_coro_pth_func_ptr
#define csnip_coro_new		csnip_coro_pth_new
#define csnip_coro_set_func	csnip_coro_pth_set_func
#define csnip_coro_next		csnip_coro_pth_next
#define csnip_coro_yield	csnip_coro_pth_yield
#define csnip_coro_get_value	csnip_coro_pth_get_value
#define csnip_coro_free		csnip_coro_pth_free
#else
#error No coroutine backend implementation was configured.
#endif
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

struct csnip_coro_s;

/**	Opaque coroutine type. */
typedef struct csnip_coro_s csnip_coro;

/**	Coroutine entrypoint typedef. */
typedef void* (*csnip_coro_func_ptr)(csnip_coro*, void*);

/**	Create a coroutine handle.
 *
 *	This function allocates the underlying context necessary to
 *	execute the coroutine.  No coroutine is started yet; to acutally
 *	start a coroutine, a subsequent csnip_coro_set_func() followed
 *	by csnip_coro_next() is necessary.
 *
 *	Note that creation of the coroutine handle can be a relatively
 *	expensive operation, more so than setting the entry point or
 *	iterating.
 */
csnip_coro* csnip_coro_new(void);

/**	Set the coroutine entry point.
 *
 *	Set the function to be called and its arguments upon the next
 *	csnip_coro_next().  This assumes that the handle is not already
 *	iterating a coroutine (either there were no previous iterations,
 *	or the previous routine has stopped).
 */
int csnip_coro_set_func(csnip_coro* C, csnip_coro_func_ptr fptr, void* arg);

/**	Partially execute a coroutine.
 *
 *	@param		C
 *			the coroutine instance.
 *
 *	@param[in]	in
 *			The next void* pointer to pass into the
 *			coroutine.  The coroutine will see this value as
 *			the return value of the corresponding
 *			csnip_coro_yield(), if any.  It also can obtain
 *			it by calling csnip_coro_get_value().
 *
 *	@param[out]	out
 *			If non-null, the value that the coroutine sets
 *			by calling csnip_coro_yield() will be assigned
 *			into *out.
 *
 *	@return		0	if iteration continues,
 *			1	if the coroutine has terminated,
 *			< 0	a csnip error code in case of an error.
 */
int csnip_coro_next(csnip_coro* C, void* in, void** out);

/**	Yield execution from within a coroutine. */
void* csnip_coro_yield(csnip_coro* C, void* val);

/**	Retrieve the value passed to or from the coroutine.
 *
 *	From within the coroutine, this returns the value passed to the
 *	coroutine via csnip_coro_next(), which is the last value
 *	returned by csnip_coro_yield().  When called in a coroutine
 *	before the first call to csnip_coro_yield(), the value passed in
 *	the very first csnip_coro_next() call is passed.
 *
 *	From outside of the coroutine, this function returns the value
 *	passed from the coroutine to the main procedure via
 *	csnip_coro_yield().
 */
void* csnip_coro_get_value(csnip_coro* C);

/**	Free a coroutine handle. */
void csnip_coro_free(csnip_coro* C);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CSNIP_CORO_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_CORO_HAVE_SHORT_NAMES)
#define coro_s		csnip_coro_s
#define coro		csnip_coro
#define coro_func_ptr	csnip_coro_func_ptr
#define coro_new	csnip_coro_new
#define coro_set_func	csnip_coro_set_func
#define	coro_next	csnip_coro_next
#define coro_yield	csnip_coro_yield
#define coro_get_value	csnip_coro_get_value
#define coro_free	csnip_coro_free
#define CSNIP_CORO_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_CORO_HAVE_SHORT_NAMES */
