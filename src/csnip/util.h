#ifndef CSNIP_UTIL_H
#define CSNIP_UTIL_H

/** @file util.h
 *  @defgroup util		Utilities
 *  @{
 *
 *  Simple utilities that are frequently used.
 *
 *  The macros provided here provide very simple functionality that we
 *  have all seen defined and used many times.  It would be better to
 *  keep reusing an existing definition of such things and so we provide
 *  one.
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>

/**  Swap two variables, type explicitly specified. */
#define csnip_Tswap(type, a, b) \
	do { \
		type csnip_tmp =(a); \
		(a) = (b); \
		(b) = csnip_tmp; \
	} while(0)

/** Swap two variables.
 *
 *  @note This assumes the typeof(.) keyword is available, which is not in any
 *	  of the C standards but known by many, but not all, compilers.
 */
#define csnip_Swap(a, b) \
	do { \
		typeof(a) csnip_tmp = (a); \
		(a) = (b); \
		(b) = csnip_tmp; \
	} while(0)

/** Swap two variables using memcpy().
 *
 *  This works assuming the variables in question are not register
 *  variables.
 */
#define csnip_Cswap(a, b) \
	do { \
		_Static_assert(sizeof(a) == sizeof(b)); \
		char csnip_tmp[sizeof(a)]; \
		memcpy(csnip_tmp, &(a), sizeof(csnip_tmp)); \
		(a) = (b); \
		memcpy(&(b), csnip_tmp, sizeof(csnip_tmp)); \
	} while(0)

/** Minimum of 2 values. */
#define csnip_Min(a, b) \
		((a) < (b) ? (a) : (b))

/** Maximum of 2 values. */
#define csnip_Max(a, b) \
		((a) > (b) ? (a) : (b))

/** Clamp val between lb and ub.
 *
 *  If val is smaller than lb, return lb, if it is larger then ub,
 *  return ub.  Otherwise, return val.
 */
#define csnip_Clamp(lb, val, ub) \
		((val) < (lb) ? (lb) : ((val) > (ub) ? (ub) : (val)))

/** Length of a C static array. */
#define csnip_Static_len(a) (sizeof(a) / sizeof(*(a)))

/** Find the containing struct of a pointer to a member.
 *
 *  @param	ptr
 *		pointer to a member variable.
 *
 *  @param	type
 *		The type of the container.
 *
 *  @param	member
 *		The name of the container member.
 */
#define csnip_Container_of(ptr, type, member) \
	((type*)(((char*)ptr) - offsetof(type, member)))

/** Compute the next power of 2 of a number. */
inline size_t csnip_next_pow_of_2(size_t a)
{
	if (a == 0)
		return 1;
	--a;
	a |= a >> 1;
	a |= a >> 2;
	a |= a >> 4;
	a |= a >> 8;
	a |= a >> 16;
#if SIZE_MAX > 0xFFFFFFFF
	a |= a >> 32;
#endif
	++a;
	return a;
}

/**	Fill a range with values.
 *
 *	Set dest[0], ..., dest[len - 1] to the value val.
 *
 *	len is evaluated once; dest and val are evaluated "len" times,
 *	once for each assignment.  Assignments are made in increasing
 *	index order.
 */
#define csnip_Fill_n(dest, len, val) \
	do { \
		size_t csnip__n = (size_t)(len); \
		size_t csnip__i = 0; \
		for (; csnip__i < csnip__n; ++csnip__i) { \
			(dest)[csnip__i] = (val); \
		} \
	} while (0)

/**	Fill a range with values.
 *
 *	This is a version of Fill_n which takes an (exclusive) end
 *	pointer instead of a length.
 */
#define csnip_Fill(dest_begin, dest_end, val) \
	csnip_Fill_n(dest_begin, (dest_end) - (dest_begin), val)

/**	Copy a range from src to dest.
 *
 *	Copy src[0], ..., src[len - 1] to dest[0], ..., dest[len - 1].
 *
 *	len is evaluated once, while src and dest are evaluated once per
 *	assignment.
 */
#define csnip_Copy_n(src, len, dest) \
	do { \
		size_t csnip__n = (size_t)(len); \
		size_t csnip__i = 0; \
		for (; csnip__i < csnip__n; ++csnip__i) \
		{ \
			(dest)[csnip__i] = (src)[csnip__i]; \
		} \
	} while (0)

/**	Copy a range from src to dest.
 *
 *	This is a version of Copy_n which takes an (exclusive) end
 *	pointer instead of a length.
 */
#define csnip_Copy(src_begin, src_end, dest) \
	csnip_Copy_n(src_begin, (src_end) - (src_begin), dest)

/** @} */

#endif /* CSNIP_UTIL_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_UTIL_HAVE_SHORT_NAMES)
#define Tswap			csnip_Tswap
#define Swap			csnip_Swap
#define Cswap			csnip_Cswap
#define Min			csnip_Min
#define Max			csnip_Max
#define Clamp			csnip_Clamp
#define Static_len		csnip_Static_len
#define Container_of		csnip_Container_of
#define next_pow_of_2		csnip_next_pow_of_2
#define Fill_n			csnip_Fill_n
#define Fill			csnip_Fill
#define Copy_n			csnip_Copy_n
#define Copy			csnip_Copy
#define CSNIP_UTIL_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_UTIL_HAVE_SHORT_NAMES */
