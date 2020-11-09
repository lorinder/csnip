#ifndef CSNIP_TIME_H
#define CSNIP_TIME_H

/**	@file time.h
 *	@brief				Time utilities
 *	@defgroup time			Time utilities
 *	@{
 *
 *	@brief Utilities to deal with time.
 *
 *	The C language and POSIX have several ways to represent times,
 *	in particular time_t, struct timespec, struct timeval.
 *	Furthermore it is often convenient to use floating point values
 *	to represent times.  This module provides a number of functions
 *	and macros to conveniently convert between the different time
 *	representations, as well as functions for sleeping, comparison
 *	of times, addition and subtraction of times.  The module does
 *	not, however, get times; e.g., we do not duplicate functionality
 *	for clock_gettime().
 *
 *	The macro implementation is generic and can transparently handle
 *	the correct representation of time.
 *
 *	*Representations*.  struct timespec and struct timeval have
 *	reasonably well defined semantics.  In C11, the time_t
 *	representation is implementation defined.  This module assumes
 *	that time_t has the POSIX encoding, i.e. it is the amount of
 *	seconds elapsed since the epoch (ignoring leap seconds), where
 *	the epoch is the same as the one struct timespec is anchored to.
 *	The macros further assume that time_t is not a floating point
 *	type.  (We're not aware of any libc where time_t is a floating
 *	type, but the C standard does not exclude the possibility.)
 *	Time stored in floating point values is taken to count the
 *	number of seconds since the epoch.
 *
 *	*Precision*.  struct timespec is considered the canonical
 *	representation of time; it has a resolution of 1 nanosecond.  A
 *	double value can under some circumstances have better resolution
 *	than that, e.g. for the typical 64 bit double with 53 bit
 *	mantissa, a time smaller than 52 days has better precision than
 *	a nanosecond.  This module does not take advantage of that fact,
 *	even those small doubles will still only have precision of
 *	struct timespec.
 *
 *	*Assumptions*.  We assume that time_t and the tv_sec member of
 *	struct timespec have the same representation; the C11 standard
 *	does not guarantee this.  The macros also assume C11's _Generic,
 *	and that time_t is not a float type.
 */

#include <time.h>

#include <csnip/err.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeval;

/** @name Conversion to struct timespec. */
/**@{*/
/** time_t -> timespec */
struct timespec csnip_time_time_t_as_timespec(time_t t);
/** float -> timespec */
struct timespec csnip_time_float_as_timespec(float f);
/** double -> timespec */
struct timespec csnip_time_double_as_timespec(double d);
/** long double -> timespec */
struct timespec csnip_time_ldouble_as_timespec(long double d);
/** timeval -> timespec */
struct timespec csnip_time_timeval_as_timespec(struct timeval tv);
/**@}*/


/** @name Conversion from struct timespec. */
/**@{*/
/** timespec -> time_t */
time_t csnip_time_timespec_as_time_t(struct timespec ts);
/** timespec -> float */
float csnip_time_timespec_as_float(struct timespec ts);
/** timespec -> double */
double csnip_time_timespec_as_double(struct timespec ts);
/** timespec -> long double */
long double csnip_time_timespec_as_ldouble(struct timespec ts);
/** timespec -> timeval */
struct timeval csnip_time_timespec_as_timeval(struct timespec ts);
/**@}*/

/** Sleep with subsecond precision. */
int csnip_time_sleep(struct timespec ts);
/** Less-than comparison. */
int csnip_time_is_less(struct timespec a, struct timespec b);
/** Less-than-or-equal comparison. */
int csnip_time_is_less_equal(struct timespec a, struct timespec b);
/** Compute the sum of two durations. */
struct timespec csnip_time_add(struct timespec a, struct timespec b);
/** Compute the difference of two durations. */
struct timespec csnip_time_sub(struct timespec a, struct timespec b);

#ifdef __cplusplus
}
#endif

/**	Convert the given time quantity to a struct timespec.
 *
 *	This expression-macro converts the time given in src to a
 *	timespec.  It accepts struct timeval, time_t, or a floating
 *	point type as input type.  Floating point numbers are considered
 *	as second values.
 */
#define csnip_time_AsTimespec(src) \
	csnip_time__AsTimespec((src))
/** @cond */
#define csnip_time__AsTimespec(src) \
	_Generic(src, \
	  time_t: \
	    csnip_time_time_t_as_timespec( \
		_Generic(src, time_t: src, default: 0)), \
	  float: \
	    csnip_time_float_as_timespec( \
		_Generic(src, float: src, default: 0)), \
	  double: \
	    csnip_time_double_as_timespec( \
		_Generic(src, double: src, default: 0)), \
	  long double: \
	    csnip_time_ldouble_as_timespec( \
		_Generic(src, long double: src, default: 0)), \
	  struct timeval: \
	    csnip_time_timeval_as_timespec( \
	      _Generic(src, struct timeval: src, \
			default: (struct timeval){ 0 })), \
	  struct timespec: src)
/** @endcond */

/**	Convert a time.
 *
 *	This statement-macro converts the time represented in @a src to
 *	the format of @a target and stores it in target.  Acceptable
 *	types for both @a src and @a target are struct timespec, struct
 *	timeval, time_t, or a floating point value.
 */
#define csnip_time_Convert(src, target) \
	csnip_time__Convert((src), (target), csnip__tmp_ts)
/** @cond */
#define csnip_time__Convert(src, target,		tmp) \
	do { \
		struct timespec tmp = \
		  csnip_time_AsTimespec(src); \
		target = _Generic(target, \
		  time_t: \
		    csnip_time_timespec_as_time_t(tmp), \
		  float: \
		    csnip_time_timespec_as_float(tmp), \
		  double: \
		    csnip_time_timespec_as_double(tmp), \
		  long double: \
		    csnip_time_timespec_as_ldouble(tmp), \
		  struct timeval: \
		    csnip_time_timespec_as_timeval(tmp), \
		  struct timespec: \
		    tmp); \
	} while (0)
/** @endcond */

/**	Sleep with subsecond precision.
 *
 *	The sleep statement macro sleeps for the given amount; any
 *	supported representation can be used for @a amount.
 */
#define csnip_time_Sleep(amount, err) \
	csnip_time__Sleep((amount), (err), csnip__cerr)
/** @cond */
#define csnip_time__Sleep(amount, err,		cerr) \
	do { \
		const int cerr = csnip_time_sleep( \
		  csnip_time_AsTimespec(amount)); \
		if (cerr != 0) { \
			csnip_err_Raise(cerr, err); \
		} \
	} while (0)
/** @endcond */

/**	Check if time_a < time_b.
 *
 *	Expression-macro to compare times.  Any supported representation
 *	can be used for @a time_a and @a time_b and they don't need to
 *	be the same representation.
 */
#define csnip_time_IsLess(time_a, time_b) \
	csnip_time_is_less( \
	  csnip_time_AsTimespec(time_a), \
	  csnip_time_AsTimespec(time_b))

/**	Check if time_a <= time_b.
 *
 *	Expression-macro to compare times.  Any supported representation
 *	can be used for @a time_a and @a time_b and they don't need to
 *	be the same representation.
 */
#define csnip_time_IsLessEqual(time_a, time_b) \
	csnip_time__IsLessEqual((time_a), (time_b))
/** @cond */
#define csnip_time__IsLessEqual(time_a, time_b) \
	csnip_time_is_less_equal( \
	  csnip_time_AsTimespec(time_a), \
	  csnip_time_AsTimespec(time_b))
/** @endcond */

/**	Compute the sum of two times.
 *
 *	Expression-macro to compute @a time_a + @a time_b.  Any
 *	supported representation can be used for @a time_a and @a time_b
 *	and they don't need to be the same representation.
 */
#define csnip_time_Add(time_a, time_b) \
	csnip_time_add( \
	  csnip_time_AsTimespec(time_a), \
	  csnip_time_AsTimespec(time_b))

/**	Compute the difference of two times.
 *
 *	Expression-macro to compute @a time_a - @a time_b.  Any
 *	supported representation can be used for @a time_a and @a time_b
 *	and they don't need to be the same representation.
 */
#define csnip_time_Sub(time_a, time_b) \
	csnip_time_sub( \
	  csnip_time_AsTimespec(time_a), \
	  csnip_time_AsTimespec(time_b))

/** @} */

#endif /* CSNIP_TIME_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_TIME_HAVE_SHORT_NAMES)
#define time_time_t_as_timespec		csnip_time_time_t_as_timespec
#define time_float_as_timespec		csnip_time_float_as_timespec
#define time_double_as_timespec		csnip_time_double_as_timespec
#define time_ldouble_as_timespec	csnip_time_ldouble_as_timespec
#define time_timeval_as_timespec	csnip_time_timeval_as_timespec
#define time_timespec_as_time_t		csnip_time_timespec_as_time_t
#define time_timespec_as_float		csnip_time_timespec_as_float
#define time_timespec_as_double		csnip_time_timespec_as_double
#define time_timespec_as_ldouble	csnip_time_timespec_as_ldouble
#define time_timespec_as_timeval	csnip_time_timespec_as_timeval
#define time_sleep			csnip_time_sleep
#define time_is_less			csnip_time_is_less
#define time_is_less_equal		csnip_time_is_less_equal
#define time_add			csnip_time_add
#define time_sub			csnip_time_sub
#define time_AsTimespec			csnip_time_AsTimespec
#define time_Convert			csnip_time_Convert
#define time_Sleep			csnip_time_Sleep
#define time_IsLess			csnip_time_IsLess
#define time_IsLessEqual		csnip_time_IsLessEqual
#define time_Add			csnip_time_Add
#define time_Sub			csnip_time_Sub
#define CSNIP_TIME_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_TIME_HAVE_SHORT_NAMES */
