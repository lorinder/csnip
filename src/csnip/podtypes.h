#ifndef CSNIP_PODTYPES_H
#define CSNIP_PODTYPES_H

/** @file podtypes.h
 *  @defgroup podtypes		Typedefs for PODs
 *  @{
 *
 *  Typedefs for plain old data types.  These are useful as shorthands.
 */

#include <stdint.h>

/* Various unsigned types */

typedef unsigned char csnip_uchar;		/**< unsigned char */
typedef unsigned short int csnip_ushort;	/**< unsigned short */
typedef unsigned int csnip_uint;		/**< unsigned int */
typedef unsigned long csnip_ulong;		/**< unsigned long */

/* long long */
typedef long long int csnip_llong;		/**< long long */
typedef unsigned long long int csnip_ullong;	/**< unsigned long long */

/* floating point */
typedef long double csnip_ldouble;		/**< long double */

/* pointers */
typedef char* csnip_pchar;			/**< char* */
typedef void* csnip_pvoid;			/**< void* */

#ifdef CSNIP_SHORT_NAMES
#define	uchar		csnip_uchar
#define ushort		csnip_ushort
#define uint		csnip_uint
#define ulong		csnip_ulong
#define llong		csnip_llong
#define ullong		csnip_ullong
#define ldouble		csnip_ldouble
#define pchar		csnip_pchar
#define pvoid		csnip_pvoid
#endif

/** @} */

#endif /* CSNIP_PODTYPES_H */
