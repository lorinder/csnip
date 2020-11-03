#ifndef CSNIP_SORT_H
#define CSNIP_SORT_H

/**  @file sort.h
 *   @brief			Sorting algorithms
 *   @defgroup sort		Sorting algorithms
 *   @{
 *
 *   Comparison based sorting algorithms.
 */

#include <stddef.h>

#include <csnip/heap.h>
#include <csnip/preproc.h>

/* Qsort parameters */

#ifndef CSNIP_QSORT_STACKSZ
/**   Size of the Qsort stack.  */
#define CSNIP_QSORT_STACKSZ	64
#endif

#ifndef CSNIP_QSORT_SLIMIT
/**  Minimum Qsort partition size.
 *
 *   Partitioning is no longer applied below the minimum size.
 *   Instead, another simple sorting algorithnm will be used.
 */
#define CSNIP_QSORT_SLIMIT	24
#elif CSNIP_QSORT_SLIMIT < 3
/* The pivot selection algorithm does not deal with an SLIMIT too
 * small.
 */
#error "CSNIP_QSORT_SLIMIT must be 3 or larger."
#endif

/**  Compute median3 pivot (for Quicksort).
 *
 *   Computes a median-of-three pivot (first, middle and last
 *   element), and moves it to the beginning of the range.
 *
 *   @param	u, v
 *		dummy variables, representing array indices.
 *
 *   @param	au_lessthan_av
 *		Expression to check whether a[u] < a[v], where a is the
 *		backing array.
 *
 *   @param	swap_au_av
 *		Statement to swap the u-th and v-th element in the
 *		backing array.
 *
 *   @param	beg
 *		index of the first element to partition.
 *
 *   @param	end
 *		index one past the last element in the range.
 */
#define csnip_Qsort_median3_pivot(u, v, au_lessthan_av, swap_au_av, beg, end) \
	do { \
	/* Median 3 pivot selection. \
	 * The following steps will place the pivot at \
	 * the beginning. \
	 */ \
	const size_t csnip_qs_mid = beg + (end - beg)/2; \
	size_t u, v; \
	u = end - 1; \
	v = csnip_qs_mid; \
	if (au_lessthan_av) { \
		swap_au_av; \
	} \
	/* At this point, we have middle <= end. */ \
	u = beg; \
	if (au_lessthan_av) { \
		/* beg < middle <= end  \
		 * ==> Use middle as pivot. \
		 */ \
		swap_au_av; \
	} else { \
		/* middle <= beg. \
		 * might have middle <= beg <= end, or \
		 * middle <= end <= beg. \
		 */ \
		v = end - 1; \
		if (au_lessthan_av) { \
			/* middle <= beg <= end. \
			 * Median is already correctly \
			 * placed. \
			 */ \
		} else { \
			/* middle <= end <= beg. \
			 * => Use end as pivot. \
			 */ \
			swap_au_av; \
		} \
	} \
	} while (0)

/**  Quicksort's partition algorithm.
 *
 *   The pivot is the first element in the range.  After execution of
 *   the algorithm, the given range is partitioned, such
 *
 *   1.	The formerly first array element a[beg] is now at a[result].
 *
 *   2.	All array entries with index < result are <= a[result].
 *
 *   3.	All array entries with index > result are >= a[result].
 *
 *   @param	u,v
 *		dummy variables
 *
 *   @param	au_lessthan_av
 *		Expression evaluation a[u] < a[v].
 *
 *   @param	swap_au_av
 *		Statement to swap a[u] and a[v].
 *
 *   @param	beg
 *		First index in the range to partition.
 *
 *   @param	end
 *		Last index in the range to partition.
 *
 *   @param	result
 *		l-value where the location of the pivot is returned.
 *
 */
#define csnip_Qsort_partition(u, v, au_lessthan_av, swap_au_av, beg, end, result) \
	do { \
	\
	/* Separate */ \
	size_t u, v; \
	size_t csnip__qs_lo = beg; \
	size_t csnip__qs_hi = end; \
	do { \
		/* Ascend in lower partition */ \
		u = csnip__qs_lo; \
		v = beg; \
		do { \
			++u; \
		} while (au_lessthan_av); \
		csnip__qs_lo = u; \
		\
		/* Descend in higher partition */ \
		u = beg; \
		v = csnip__qs_hi; \
		do { \
			--v; \
		} while (au_lessthan_av); \
		csnip__qs_hi = v; \
		\
		/* Terminate if ends meet */ \
		if (csnip__qs_hi <= csnip__qs_lo) \
			break; \
		\
		/* Exchange */ \
		u = csnip__qs_lo; \
		/* v = csnip__qs_hi; */ \
		swap_au_av; \
	} while(1); \
	\
	/* Move pivot in place */ \
	u = beg; \
	v = csnip__qs_hi; \
	swap_au_av; \
	\
	(result) = csnip__qs_hi; \
	} while(0)

/**  Quicksort algorithm.
 *
 *   The classic median-of-three quicksort algorithm.  This is a very
 *   fast sorting algorithm, running in O(N log N) time in typical
 *   cases, but has pathological cases of O(N^2).
 *
 *   @param	u, v
 *		dummy variables
 *
 *   @param	au_lessthan_av
 *		Comparator expression, evaluates to true if a[u] < a[v].
 *
 *   @param	swap_au_av
 *		Statement to swap a[u] with a[v].
 *
 *   @param	N
 *		Size of the array to sort.
 */
#define csnip_Qsort(u, v, au_lessthan_av, swap_au_av, N) \
	do { \
	\
	int csnip_qs_n = 0; \
	size_t csnip_qs_sbeg[CSNIP_QSORT_STACKSZ]; \
	size_t csnip_qs_send[CSNIP_QSORT_STACKSZ]; \
	if ((N) > CSNIP_QSORT_SLIMIT) { \
		++csnip_qs_n; \
		csnip_qs_sbeg[0] = 0; \
		csnip_qs_send[0] = (N); \
	} \
	\
	/* Partitioning iteration */ \
	while(csnip_qs_n > 0) { \
		--csnip_qs_n; \
		const size_t csnip_qs_beg = csnip_qs_sbeg[csnip_qs_n]; \
		const size_t csnip_qs_end = csnip_qs_send[csnip_qs_n]; \
		\
		/* Put the median to the start */ \
		csnip_Qsort_median3_pivot(u, v, au_lessthan_av, swap_au_av, \
			csnip_qs_beg, csnip_qs_end); \
		\
		/* Partition */ \
		size_t csnip_p; \
		csnip_Qsort_partition(u, v, au_lessthan_av, swap_au_av, \
			csnip_qs_beg, csnip_qs_end, csnip_p); \
		\
		/* Put search subregions on stack */ \
		const size_t csnip_d1 = csnip_p - 1 - csnip_qs_beg; \
		const size_t csnip_d2 = csnip_qs_end - csnip_p - 1; \
		if (csnip_d1 > csnip_d2) { \
			if (csnip_d1 > CSNIP_QSORT_SLIMIT) { \
				csnip_qs_sbeg[csnip_qs_n] = csnip_qs_beg; \
				csnip_qs_send[csnip_qs_n++] = csnip_p; \
				if (csnip_d2 > CSNIP_QSORT_SLIMIT) { \
					csnip_qs_sbeg[csnip_qs_n] = \
					  csnip_p + 1; \
					csnip_qs_send[csnip_qs_n++] = \
					  csnip_qs_end; \
				} \
			} \
		} else { \
			if (csnip_d2 > CSNIP_QSORT_SLIMIT) { \
				csnip_qs_sbeg[csnip_qs_n] = \
				  csnip_p + 1; \
				csnip_qs_send[csnip_qs_n++] = \
				  csnip_qs_end; \
				if  (csnip_d1 > CSNIP_QSORT_SLIMIT) { \
					csnip_qs_sbeg[csnip_qs_n] = \
					  csnip_qs_beg; \
					csnip_qs_send[csnip_qs_n++] = \
					  csnip_p; \
				} \
			} \
		} \
	} \
	\
	/* Clean up remaining disorder */ \
	/* At this point, the data is close to sorted, but partitions of size \
	 * CSNIP_QSORT_SLIMIT or smaller have not been put into their \
	 * correct order.  We use Shellsort to finish up. \
	 */ \
	if (CSNIP_QSORT_SLIMIT > 1) { \
		static const size_t csnip_gs[] = {1}; \
		csnip_ShellsortGS(u, v, au_lessthan_av, \
		  swap_au_av, N, csnip_Static_len(csnip_gs), \
		  csnip_gs); \
	} \
	\
	} while(0)

#ifndef CSNIP_HEAPSORT_K
/**   Heap arity for sorting algorithm. */
#define CSNIP_HEAPSORT_K	2
#endif

/**  Heapsort algorithm.
 *
 *   Sorting algorithm.  Is O(N log N) worst case, but usually
 *   significantly slower than Quicksort.
 *
 *   @param	u, v
 *		dummy variables
 *
 *   @param	au_lessthan_av
 *		Comparator expression
 *
 *   @param	swap_au_av
 *		Statement to swap a[u] with a[v]
 *
 *   @param	N
 *		Array size
 */
#define csnip_Heapsort(u, v, au_lessthan_av, swap_au_av, N) \
	do { \
		if ((N) <= 1) \
			break; \
		csnip_heap_Heapify(v, u, au_lessthan_av, swap_au_av, \
			CSNIP_HEAPSORT_K, N); \
		size_t csnip__heapsort_i = (N) - 1; \
		while (1) { \
			{ \
				size_t u = 0, v = csnip__heapsort_i; \
				swap_au_av; \
			} \
			if (csnip__heapsort_i <= 1) \
				break; \
			csnip_heap_SiftDown(v, u, \
			  au_lessthan_av, swap_au_av, \
			  CSNIP_HEAPSORT_K, csnip__heapsort_i, 0); \
			--csnip__heapsort_i; \
		} \
	} while(0)

/**  Shellsort algorithm.
 *
 *   This sorting algorithm has unknown complexity that lies
 *   somewhere between O(N log N) and O(N^2).  It is very simple, and
 *   very useful when code size matters.  In practice, its run time
 *   behaviour is similar to heapsort for small to medium sized
 *   arrays.
 *
 *   @param	u, v
 *		dummy variables
 *
 *   @param	au_lessthan_av
 *		Comparator expression
 *
 *   @param	swap_au_av
 *		Statement to swap a[u] with a[v]
 *
 *   @param	N
 *		Array size
 */
#define csnip_Shellsort(u, v, au_lessthan_av, swap_au_av, N) \
	do { \
		int csnip_gap = ((N) > 4 ? (N) / 4 : 1); \
		while (csnip_gap > 0) { \
			int csnip_j; \
			for (csnip_j = csnip_gap; csnip_j < (N); ++csnip_j) { \
				int u = csnip_j, v = csnip_j - csnip_gap; \
				while (v >= 0 && (au_lessthan_av)) { \
					swap_au_av; \
					u -= csnip_gap; \
					v -= csnip_gap; \
				} \
			} \
			\
			/* next gap */ \
			if (csnip_gap == 2) \
				csnip_gap = 3; \
			csnip_gap = 4*csnip_gap / 9; \
		} \
	} while (0)

/**  Shellsort with gap sequence.
 *
 *   This variant of shellsort allows for explicitly specified gap
 *   sequences.
 *
 */
#define csnip_ShellsortGS(u, v, au_lessthan_av, swap_au_av, N, nGaps, gapSeq) \
	do { \
		int csnip_gi = 0; \
		while (csnip_gi < (int)(nGaps)) { \
			const int csnip_gap = (gapSeq)[csnip_gi]; \
			int csnip_j; \
			for (csnip_j = csnip_gap; csnip_j < (N); ++csnip_j) { \
				int u = csnip_j, v = csnip_j - csnip_gap; \
				while (v >= 0 && (au_lessthan_av)) { \
					swap_au_av; \
					u -= csnip_gap; \
					v -= csnip_gap; \
				} \
			} \
			\
			++csnip_gi; \
		} \
	} while (0)

/**  Check if an array is sorted.
 *
 *   @param	u, v
 *		Dummy variables.
 *
 *   @param	au_lessthan_av
 *		Comparator expression.
 *
 *   @param	N
 *		Array size.
 *
 *   @param	ret
 *		l-value to return the result.  Will be 1 if sorted, 0 if
 *		not.
 */
#define csnip_IsSorted(u, v, au_lessthan_av, N, ret) \
	do { \
		int u = 1; \
		(ret) = 1; \
		while (u < (N)) { \
			const int v = u - 1; \
			if (au_lessthan_av) { \
				ret = 0; \
				break; \
			} \
			++u; \
		} \
	} while (0)

/**  Declare sorting functions.
 *
 *   Generator macro to create prototypes for specific instances of
 *   the sorting functions.
 *
 *   @param	scope
 *		Scope to use for the function declaration.
 *
 *   @param	prefix
 *		Prefix for the function names to be generated.
 *
 *   @param	gen_args
 *		Generic argument list to be provided to the functions,
 *		of the form args(...) or noargs().
 */
#define CSNIP_SORT_DECL_FUNCS(scope, prefix, gen_args) \
	scope void prefix ## qsort(csnip_pp_list_##gen_args) \
	scope void prefix ## heapsort(csnip_pp_list_##gen_args) \
	scope void prefix ## shellsort(csnip_pp_list_##gen_args) \
	scope int prefix ## is_sorted(csnip_pp_list_##gen_args)

/**  Define sorting functions.
 *
 *   Generator macro to create for specific sorting functions.
 *
 *   @param	scope
 *		Scope to use for the function declaration.
 *
 *   @param	prefix
 *		Prefix for the function names to be generated.
 *
 *   @param	gen_args
 *		Generic argument list to be provided to the functions,
 *		of the form args(...) or noargs().
 *
 *   @param	u, v
 *		dummy variables
 *
 *   @param	au_lessthan_av
 *		comparator expression
 *
 *   @param	swap_au_av
 *		swapping statement
 *
 *   @param	N
 *		array size
 */
#define CSNIP_SORT_DEF_FUNCS(scope, prefix, gen_args, \
				u, v, au_lessthan_av, swap_au_av, N) \
	\
	scope void prefix ## qsort(csnip_pp_list_##gen_args) \
	{ \
		csnip_Qsort(u, v, au_lessthan_av, swap_au_av, N); \
	} \
	\
	scope void prefix ## heapsort(csnip_pp_list_##gen_args) \
	{ \
		csnip_Heapsort(u, v, au_lessthan_av, swap_au_av, N); \
	} \
	\
	scope void prefix ## shellsort(csnip_pp_list_##gen_args) \
	{ \
		csnip_Shellsort(u, v, au_lessthan_av, swap_au_av, N); \
	} \
	\
	scope int prefix ## is_sorted(csnip_pp_list_##gen_args) \
	{ \
		int ret; \
		csnip_IsSorted(u, v, au_lessthan_av, N, ret); \
		return ret; \
	}

#ifdef CSNIP_SHORT_NAMES
#define Qsort			csnip_Qsort
#define Heapsort		csnip_Heapsort
#define Shellsort		csnip_Shellsort
#define IsSorted		csnip_IsSorted
#endif

/** @} */

#endif /* CSNIP_SORT_H */
