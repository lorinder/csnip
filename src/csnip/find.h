#ifndef CSNIP_FIND_H
#define CSNIP_FIND_H

/** @file find.h
 *  @brief			Search functions
 *  @defgroup	find		Search functions
 *  @{
 *
 *  Binary search algorithm.
 *
 *  The csnip_Bsearch() macro provides a binary seach facility that
 *  improves on libc's bsearch interface in a number of ways:
 *
 *  1.  bsearch() can only be used to find exact matches, or determine
 *      whether there is a match.  Frequently (or, dare I say, usually),
 *      however, one wants to find the closest value in the array, not
 *      necessarily an exact match.  For such cases, bsearch doesn't
 *      work, but csnip_Bsearch() can be used.
 *
 *  2.  Easier to use interface.  Bsearch requires implementation of a
 *      separate comparison function, and is not type safe;
 *      csnip_Bsearch() does not have these problems.
 *
 *  3.	Has the potential to be faster than bsearch() because it's not
 *      necessary to dereference a function pointer for each comparison.
 */

/** Binary search.
 *
 *  Statement macro. Find the smallest index i in a sorted array such
 *  that the i-th entry is at least as large as a specified key.  If no
 *  such index exists return N, the size of the array.
 *
 *  @param	itype
 *		integral type used for the return value. itype is also
 *		used for indexing.
 *
 *  @param	u
 *		dummy variable for the comparison expression.
 *
 *  @param	au_lessthan_key
 *		expression in u that evaluates to true if the u-th entry
 *		of the array is less than the key, and false otherwise.
 *
 *  @param	N
 *		the size of the array.
 *
 *  @param	ret
 *		lvalue of type itype to store the result index.
 *
 *  @note	By default the csnip_Bsearch function finds the
 *		lowest index i such that a[i] >= key. To find
 *		the lowest index such that a[i] > key, the
 *		au_lessthan_key expression can be set to a
 *		less-than-or-equal expression.
 */
#define csnip_Bsearch(itype, u, au_lessthan_key, N, ret) \
	do { \
		/* loop invariants:
		 * a[0], ..., a[lo - 1] < key;
		 * a[hi], ..., a[N - 1] >= key;
		 */ \
		itype lo = 0; \
		itype hi = (N); \
		while (hi != lo) { \
			itype u = lo + (hi - lo)/2; \
			if (au_lessthan_key) { \
				lo = (u) + 1; \
			} else { \
				hi = (u); \
			} \
		} \
		\
		(ret) = hi; \
	} while(0)

/** @} */

#endif /* CSNIP_FIND_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_FIND_HAVE_SHORT_NAMES)
#define Bsearch		csnip_Bsearch
#define CSNIP_FIND_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && ! CSNIP_FIND_HAVE_SHORT_NAMES */
