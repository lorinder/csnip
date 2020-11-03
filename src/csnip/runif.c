#include <assert.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <tgmath.h>

#define CSNIP_SHORT_NAMES
#include <csnip/rng.h>
#include <csnip/runif.h>

#define DEF_runif_get(type, type_max, func_name) \
type func_name(const rng* R, type max) \
{ \
	const type delta = R->maxval - R->minval; \
	if (max <= delta) { \
		if (max == type_max) { \
			/* In this case, we also have
			 * delta == type_max
			 */ \
			return rng_getnum(R); \
		} \
 \
		/* Split the possible random values entries into (max +
		 * 1) buckets of equal size b each.  Random values >= ub
		 * will not be used and resampled.  In the case where
		 * delta == type_max, we choose a somewhat pessimistic
		 * ub to avoid numeric overflow.
		 */ \
		type b; \
		if (delta == type_max) { \
			b = type_max / (max + 1); \
		} else { \
			b = (delta + 1) / (max + 1); \
		} \
		type ub = b * (max + 1); \
		type l; \
		do { \
			l = rng_getnum(R) - R->minval; \
		} while (l >= ub); \
 \
		return l / b; \
	} else { \
		/* max > delta.
		 * Want to decompose (max + 1) into (delta + 1) slices
		 * of the same size.  Since max + 1 will not need to
		 * divide delta + 1, we decompose a somewhat larger
		 * range into delta + 1 slices, and will resample if we
		 * should hit an out-of-range value in the end
		 *
		 * A first random number is going to choose a slice, a
		 * second one the entry in the slice.  If the last slice
		 * was selected, then the entry might point past the
		 * requested range, in which case the procedure has to
		 * be repeated.
		 *
		 * Ignoring possible overflows, the procedure is as follows:
		 *	b := ceil_div(max + 1, delta + 1) // slice size
		 *	do
		 *		slice := R->next_raw(R->data)
		 *		ent = runif_Geti(R, b - 1);
		 *	while (max + slice*b < ent)
		 *	return slice*b + ent
		 */ \
		const type b = (max / (delta + 1)) + 1; \
		type nSlice, nEnt; \
		do { \
			nSlice = rng_getnum(R) - R->minval; \
			nEnt = func_name(R, b - 1); \
		} while (max - nEnt < nSlice*b); \
 \
		return nSlice*b + nEnt; \
	} \
}

DEF_runif_get(unsigned int, UINT_MAX, runif_getu)
DEF_runif_get(unsigned long int, ULONG_MAX, runif_getul)
DEF_runif_get(unsigned long long int, ULLONG_MAX, runif_getull)

#undef DEF_runif_get

#define DEF_runif_getf(type, mant_dig, func_name) \
type func_name(const rng* R, type lim) \
{ \
	const type m = pow((type)FLT_RADIX, mant_dig); \
	type ret; \
	\
	/* determine a value in [0, 1) */ \
	if (ULLONG_MAX >= m - 1) { \
		/* In this case, a sufficient number of bits can be
		 * generated from a single integer.  Use the smallest
		 * sufficient integer type available. */ \
		if (ULONG_MAX >= m - 1) { \
			if (UINT_MAX >= m - 1) { \
				ret = runif_getu(R, \
					(unsigned int)(m - 1)); \
				ret /= m; \
			} else { \
				ret = runif_getul(R, \
					(unsigned long)(m - 1)); \
				ret /= m; \
			} \
		} else { \
			ret = runif_getull(R, \
					(unsigned long long)(m - 1)); \
			ret /= m; \
		} \
	} else { \
		/* ULLONG_MAX < m - 1
		 *
		 * In this case, more than one random integer needs to be
		 * generated.
		 *
		 * The below calculation of ndig is correct since
		 * ULLONG_MAX + 1 is less than m, i.e. a value which can
		 * be exactly represented.  We use log2() instead of
		 * log() to get an exact result for the common case
		 * where the radix is 2.
		 */ \
		const int ndig = \
		  (int)floor(log2(ULLONG_MAX + (type)1) \
		             / log2((type)FLT_RADIX)); \
		double ret = 0; \
		for (int i = 0; i < mant_dig; ++i) { \
			ret += (type)runif_getull(R, \
			              (unsigned long long)(m - 1)); \
			ret /= m; \
		} \
		const int u = mant_dig % ndig; \
		if (u > 0) { \
			type r = pow((type)FLT_RADIX, u); \
			ret += (type)runif_getull(R, \
					(unsigned long long)(r - 1)); \
			ret /= r; \
		} \
	} \
	\
	/* return scaled value */ \
	return ret * lim; \
}

DEF_runif_getf(long double, LDBL_MANT_DIG, runif_getld)
DEF_runif_getf(double, DBL_MANT_DIG, runif_getd)
DEF_runif_getf(float, FLT_MANT_DIG, runif_getf)

#undef DEF_runif_getf
