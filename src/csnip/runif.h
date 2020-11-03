#ifndef CSNIP_RUNIF_H
#define CSNIP_RUNIF_H

/**	@file runif.h
 *
 *	Uniform random number generation.
 */

#include <csnip/rng.h>

/* Generation of integer uniform random variables
 *
 * Each variant generates uniform integer random variables in { 0, 1,
 * ..., max }  (the upper bound, max, is inclusive).
 *
 * The generated random variables are uniform, provided that the
 * underlying random number generator produces uniform numbers.
 */
unsigned long long int csnip_runif_getull(const csnip_rng* R,
				unsigned long long int max);
unsigned long csnip_runif_getul(const csnip_rng* R,
			unsigned long max);
unsigned int csnip_runif_getu(const csnip_rng* R, unsigned int max);

#define csnip_runif_Geti(R, max) \
		_Generic((max), \
			unsigned long long int: \
			  csnip_runif_getull((R), (unsigned long long int)(max)), \
			long long int: \
			  (long long int)csnip_runif_getull((R), (long long int)(max)), \
			unsigned long int: \
			  csnip_runif_getul((R), (unsigned long int)(max)), \
			long int: \
			  (long int)csnip_runif_getul((R), (long int)(max)), \
			unsigned int: \
			  csnip_runif_getu((R), (unsigned int)(max)), \
			int: \
			  (int)csnip_runif_getu((R), (int)(max)), \
			default: \
			  csnip_runif_getu((R), (unsigned int)(max)))

/* Generation of floating point uniform random variables */
long double csnip_runif_getld(const csnip_rng* R, long double lim);
double csnip_runif_getd(const csnip_rng* R, double lim);
float csnip_runif_getf(const csnip_rng* R, float lim);

#define csnip_runif_Getf(R, lim) \
		_Generic((lim), \
			long double: csnip_runif_getld((R), (lim)), \
			double: csnip_runif_getd((R), (lim)), \
			float: csnip_runif_getf((R), (lim)))

#ifdef CSNIP_SHORT_NAMES
#define runif_getull		csnip_runif_getull
#define runif_getul		csnip_runif_getul
#define runif_getu		csnip_runif_getu
#define runif_Geti		csnip_runif_Geti
#define runif_getld		csnip_runif_getld
#define runif_getd		csnip_runif_getd
#define runif_getf		csnip_runif_getf
#define runif_Getf		csnip_runif_Getf
#endif

#endif /* CSNIP_RUNIF_H */
