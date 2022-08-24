#ifndef CSNIP_LIMITS_H
#define CSNIP_LIMITS_H

/** @file limits.h
 *  @defgroup limits		Numeric limits
 *  @{
 *
 *  Type generic numeric limit macros.
 *
 *  Macros to find the minimum and maximum range of supported integer
 *  types.  They can be useful, for example, to implement type-generic
 *  macros.
 *
 *  **Requirements**: C11, in particular support for ```_Generic```.
 */

#include <limits.h>
#include <stddef.h>

/**  Compute the limits of a given integer type.
 *
 *   Expression macro.  For an integer expression, return the limits of
 *   that type as an array of 2, containing in the first element the
 *   minimum and the second element the maximum.  For example,
 *
 *       csnip_limit_Int(42)
 *
 *   evaluates to the array
 *
 *       (const int[]){INT_MIN, INT_MAX}
 *
 */
#define csnip_limit_Int(x) csnip_limit_Int__all((x))

/** Type generic minimum value function.
 *
 *  Given an expression @a x of integer type, return the minimum
 *  value of that integer type.  For example,
 *
 *      csnip_limit_Mini(35)
 *
 *  evaluates to ```INT_MIN```, whereas
 *
 *      csnip_limit_Mini(35l)
 *
 *  evaluates to ```LONG_MIN```.
 */
#define csnip_limit_Mini(x)	(csnip_limit_Int(x)[0])

/** Type generic maximum value function.
 *
 *  Given an expression @a x of integer type, return the maximum value
 *  of that integer type.  See also csnip_limit_Mini().
 */
#define csnip_limit_Maxi(x)	(csnip_limit_Int(x)[1])

/** @cond */
#define csnip__tmm(type, minv, maxv)	type: (const type[]){ minv, maxv }

// Intma for standard types
#define csnip_limit_Int__all(x) \
	_Generic(x, csnip__tmm(signed char, SCHAR_MIN, SCHAR_MAX), default: \
	_Generic(x, csnip__tmm(unsigned char, 0, UCHAR_MAX), default: \
	_Generic(x, csnip__tmm(char, CHAR_MIN, CHAR_MAX), default: \
	_Generic(x, csnip__tmm(short int, SHRT_MIN, SHRT_MAX), default: \
	_Generic(x, csnip__tmm(unsigned short int, 0, USHRT_MAX), default: \
	_Generic(x, csnip__tmm(int, INT_MIN, INT_MAX), default: \
	_Generic(x, csnip__tmm(unsigned int, 0, UINT_MAX), default: \
	_Generic(x, csnip__tmm(long, LONG_MIN, LONG_MAX), default: \
	_Generic(x, csnip__tmm(unsigned long, 0, ULONG_MAX), default: \
	_Generic(x, csnip__tmm(long long int, LLONG_MIN, LLONG_MAX), default:\
	_Generic(x, csnip__tmm(unsigned long long int, 0, ULLONG_MAX), \
	default: csnip_limit_Int__stddef(x))))))))))))

#define csnip_limit_Int__stddef(x) \
	_Generic(x, csnip__tmm(intptr_t, INTPTR_MIN, INTPTR_MAX), default: \
	_Generic(x, csnip__tmm(uintptr_t, 0, UINTPTR_MAX), default: \
	_Generic(x, csnip__tmm(intmax_t, INTMAX_MIN, INTMAX_MAX), default: \
	_Generic(x, csnip__tmm(uintmax_t, 0, UINTMAX_MAX), default: \
	_Generic(x, csnip__tmm(ptrdiff_t, PTRDIFF_MIN, PTRDIFF_MAX), default:\
	_Generic(x, csnip__tmm(size_t, 0, SIZE_MAX), default: \
	csnip_limit_Int__i8(x) \
	))))))

#ifdef INT8_MAX
# define csnip_limit_Int__i8(x) \
	_Generic((x), \
	csnip__tmm(int8_t, INT8_MIN, INT8_MAX), \
	csnip__tmm(uint8_t, 0, UINT8_MAX), \
	default: csnip_limit_Int__i16(x))
#else
# define csnip_limit_Int__i8(x) csnip_limit_Int__i16(x)
#endif

#ifdef INT16_MAX
# define csnip_limit_Int__i16(x) \
	_Generic((x), \
	csnip__tmm(int16_t, INT16_MIN, INT16_MAX), \
	csnip__tmm(uint16_t, 0, UINT16_MAX), \
	default: csnip_limit_Int__i32(x))
#else
# define csnip_limit_Int__i16(x) csnip_limit_Int__i32(x)
#endif

#ifdef INT32_MAX
# define csnip_limit_Int__i32(x) \
	_Generic((x), \
	csnip__tmm(int32_t, INT32_MIN, INT32_MAX), \
	csnip__tmm(uint32_t, 0, UINT32_MAX), \
	default: csnip_limit_Int__i64(x))
#else
# define csnip_limit_Int__i32(x) csnip_limit_Int__i64(x)
#endif

#ifdef INT64_MAX
# define csnip_limit_Int__i64(x) \
	_Generic((x), \
	csnip__tmm(int64_t, INT64_MIN, INT64_MAX), \
	csnip__tmm(uint64_t, 0, UINT64_MAX), \
	default: csnip_limit_Int__il8(x))
#else
# define csnip_limit_Int__i64(x) csnip_limit_Int__il8(x)
#endif

#ifdef INT_LEAST8_MAX
# define csnip_limit_Int__il8(x) \
	_Generic((x), \
	csnip__tmm(int_least8_t, INT_LEAST8_MIN, INT_LEAST8_MAX), \
	csnip__tmm(uint_least8_t, 0, UINT_LEAST8_MAX), \
	default: csnip_limit_Int__il16(x))
#else
# define csnip_limit_Int__il8(x) csnip_limit_Int__il16(x)
#endif

#ifdef INT_LEAST16_MAX
# define csnip_limit_Int__il16(x) \
	_Generic((x), \
	csnip__tmm(int_least16_t, INT_LEAST16_MIN, INT_LEAST16_MAX), \
	csnip__tmm(uint_least16_t, 0, UINT_LEAST16_MAX), \
	default: csnip_limit_Int__il32(x))
#else
# define csnip_limit_Int__il16(x) csnip_limit_Int__il32(x)
#endif

#ifdef INT_LEAST32_MAX
# define csnip_limit_Int__il32(x) \
	_Generic((x), \
	csnip__tmm(int_least32_t, INT_LEAST32_MIN, INT_LEAST32_MAX), \
	csnip__tmm(uint_least32_t, 0, UINT_LEAST32_MAX), \
	default: csnip_limit_Int__il64(x))
#else
# define csnip_limit_Int__il32(x) csnip_limit_Int__il64(x)
#endif

#ifdef INT_LEAST64_MAX
# define csnip_limit_Int__il64(x) \
	_Generic((x), \
	csnip__tmm(int_least64_t, INT_LEAST64_MIN, INT_LEAST64_MAX), \
	csnip__tmm(uint_least64_t, 0, UINT_LEAST64_MAX), \
	default: csnip_limit_Int__if8(x))
#else
# define csnip_limit_Int__il64(x) csnip_limit_Int__if8(x)
#endif

#ifdef INT_FAST8_MAX
# define csnip_limit_Int__if8(x) \
	_Generic((x), \
	csnip__tmm(int_fast8_t, INT_FAST8_MIN, INT_FAST8_MAX), \
	csnip__tmm(uint_fast8_t, 0, UINT_FAST8_MAX), \
	default: csnip_limit_Int__if16(x))
#else
# define csnip_limit_Int__if8(x) csnip_limit_Int__if16(x)
#endif

#ifdef INT_FAST16_MAX
# define csnip_limit_Int__if16(x) \
	_Generic((x), \
	csnip__tmm(int_fast16_t, INT_FAST16_MIN, INT_FAST16_MAX), \
	csnip__tmm(uint_fast16_t, 0, UINT_FAST16_MAX), \
	default: csnip_limit_Int__if32(x))
#else
# define csnip_limit_Int__if16(x) csnip_limit_Int__if32(x)
#endif

#ifdef INT_FAST32_MAX
# define csnip_limit_Int__if32(x) \
	_Generic((x), \
	csnip__tmm(int_fast32_t, INT_FAST32_MIN, INT_FAST32_MAX), \
	csnip__tmm(uint_fast32_t, 0, UINT_FAST32_MAX), \
	default: csnip_limit_Int__if64(x))
#else
# define csnip_limit_Int__if32(x) csnip_limit_Int__if64(x)
#endif

#ifdef INT_FAST64_MAX
# define csnip_limit_Int__if64(x) \
	_Generic((x), \
	csnip__tmm(int_fast64_t, INT_FAST64_MIN, INT_FAST64_MAX), \
	csnip__tmm(uint_fast64_t, 0, UINT_FAST64_MAX), \
	default: csnip_limit_Int__wc(x))
#else
# define csnip_limit_Int__if64(x) csnip_limit_Int__wc(x)
#endif

#ifdef WCHAR_MAX
# define csnip_limit_Int__wc(x) \
	_Generic((x), csnip__tmm(wchar_t, WCHAR_MIN, WCHAR_MAX), \
	default: csnip_limit_Int__wi(x))
#else
# define csnip_limit_Int__wc(x) csnip_limit_Int__wi(x)
#endif

#ifdef WEOF
#define csnip_limit_Int__wi(x) \
	_Generic((x), csnip__tmm(wint_t, WINT_MIN, WINT_MAX), \
	default: csnip_limit_Int__sa(x))
#else
# define csnip_limit_Int__wi(x) csnip_limit_Int__sa(x)
#endif

#ifdef SIG_DFL
# define csnip_limit_Int__sa(x) \
	_Generic((x), csnip__tmm(sig_atomic_t, SIG_ATOMIC_MIN, SIG_ATOMIC_MAX),\
	default: 0 )
#else
# define csnip_limit_Int__sa(x) 0
#endif
/** @endcond */

/** @} */

#endif /* CSNIP_LIMITS_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_LIMITS_HAVE_SHORT_NAMES)
#define limit_Int		csnip_limit_Int
#define limit_Mini		csnip_limit_Mini
#define limit_Maxi		csnip_limit_Maxi
#define CSNIP_LIMITS_HAVE_SHORT_NAMES
#endif
