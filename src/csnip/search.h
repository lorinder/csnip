#ifndef CSNIP_SEARCH_H
#define CSNIP_SEARCH_H

/** @file search.h
 *  @brief			Search functions
 *  @defgroup	search		Search functions
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
 *  Statement macro. Find the smallest index i in an ascending sorted
 *  array a such that a[i] >= key, i.e.,  the i-th entry is at least as
 *  large as a specified key.  If no such index exists, i.e., a[i] < key
 *  for all i, return N, the size of the array.
 *
 *  @param	itype
 *		integral type used for the return value. itype is also
 *		used for indexing.
 *
 *  @param	u
 *		dummy variable for the comparison expression; this
 *		variable will be an index of type itype.
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
 *		lowest index i such that a[i] >= key.  The following
 *		alternative uses are possible:
 *
 *		- To find the lowest index i such that a[i] > key, the
 *		  au_lessthan_key expression can be set to a
 *		  less-than-or-equal expression.
 *
 *		- To find the largest index such that a[i] <= key, set
 *		  the au_lessthan_key expression to a
 *		  less-than-or-equal, and subtract 1 from the result.
 *		  Returns (itype)-1 if no such index exists.
 *
 *		- To find the largest index i such that a[i] < key, set
 *		  the au_lessthan_key expression to a less-than
 *		  expression, and subtract 1 from the result.  The
 *		  return value (itype)-1 means that no such index
 *		  exists.
 *
 *		If the array is sorted in a descending manner, the
 *		comparation expression can be reverted.
 */
#define csnip_Bsearch(itype, u, au_lessthan_key, N, ret) \
	csnip__Bsearch(itype, u, au_lessthan_key, (N), (ret), \
		csnip__Bsearch_lo, csnip__Bsearch_hi)

/** @cond */
#define csnip__Bsearch(itype, u, au_lessthan_key, N, ret, \
		lo, hi) \
	do { \
		/* loop invariants:
		 * a[0], ..., a[lo - 1] < key;
		 * a[hi], ..., a[N - 1] >= key.
		 */ \
		itype lo = 0; \
		itype hi = N; \
		while (hi != lo) { \
			itype u = lo + (hi - lo)/2; \
			if (au_lessthan_key) { \
				lo = (u) + 1; \
			} else { \
				hi = (u); \
			} \
		} \
		\
		ret = hi; \
	} while(0)
/** @endcond */

/** @} */

#endif /* CSNIP_SEARCH_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_SEARCH_HAVE_SHORT_NAMES)
#define Bsearch		csnip_Bsearch
#define CSNIP_SEARCH_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && ! CSNIP_SEARCH_HAVE_SHORT_NAMES */
