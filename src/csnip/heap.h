#ifndef CSNIP_HEAP_H
#define CSNIP_HEAP_H

/** @file heap.h
 *  @brief			Heaps
 *  @defgroup	heap		Heaps
 *  @{
 *
 *  Heaps (also known as priority queues) are data structures that allow
 *  efficient element insertion, removal, and removal of the smallest
 *  element.
 */

#include <assert.h>
#include <stddef.h>

#include <csnip/util.h>
#include <csnip/preproc.h>

/** Sift an element towards the top (root) of the heap. */
#define csnip_heap_SiftUp(u, v, au_lessthan_av, swap_au_av, K, N, i) \
	do { \
		size_t u = (size_t)(i); \
		assert(u < (N)); \
		while (u > 0) { \
			size_t v = (u - 1) / (K); \
			if (au_lessthan_av) { \
				swap_au_av; \
			} \
			u = v; \
		} \
	} while(0)

/** Sift an element towards the bottom of the heap. */
#define csnip_heap_SiftDown(u, v, au_lessthan_av, swap_au_av, K, N, i) \
	do { \
		size_t u, v; \
		size_t csnip_heap_i = (size_t)(i); \
		v = csnip_heap_i * (K) + 1; \
		while (v < (size_t)(N)) { \
			size_t csnip_heap_nu = \
				csnip_Min(v + (K), (size_t)(N)); \
			u = v + 1; \
			while (u < csnip_heap_nu) { \
				if (au_lessthan_av) \
					v = u; \
				++u; \
			} \
			u = csnip_heap_i; \
			if (au_lessthan_av) \
				break; \
			swap_au_av; \
			csnip_heap_i = v; \
			v = v * (K) + 1; \
		} \
	} while(0)

/** Transform an array into a heap. */
#define csnip_heap_Heapify(u, v, au_lessthan_av, swap_au_av, K, N) \
	do { \
		if((N) <= 1) \
			break; \
		size_t csnip_h_make_i = ((N) - 1) / (K); \
		while(1) { \
			csnip_heap_SiftDown(u, v, \
				au_lessthan_av, swap_au_av, \
				K, N, csnip_h_make_i); \
			if (csnip_h_make_i == 0) \
				break; \
			--csnip_h_make_i; \
		} \
	} while(0)

/** Check whether a given array is a heap.
 *
 *  @param[out]	ret
 *		Return value; set to "true" if the given array is a heap
 *		and to "false" if not.  In the failure case, u and v are
 *		indices to failing entries: u is the child, and v the
 *		parent such that a[u] < a[v].
 */
#define csnip_heap_Check(u, v, au_lessthan_av, swap_au_av, K, N, ret) \
	do { \
		(ret) = true; \
		if ((N) <= 1) \
			break; \
		size_t u = ((N) - 1); \
		while (u >= 1) { \
			size_t v = (u - 1) / (K); \
			if (au_lessthan_av) { \
				(ret) = false; \
				break; \
			} \
			--(u); \
		} \
	} while(0)

/** Generator macro to declare heap functions.
 *
 *  @param	scope
 *		function scope
 *
 *  @param	prefix
 *		function name prefixes
 *
 *  @param	gen_args
 *		argument list, either of the form args(...) or noargs().
 */
#define CSNIP_HEAP_DECL_FUNCS(scope, prefix, gen_args) \
	scope void prefix ## sift_up(csnip_pp_prepend_##gen_args size_t i); \
	scope void prefix ## sift_down(csnip_pp_prepend_##gen_args size_t i); \
	scope void prefix ## heapify(csnip_pp_list_##gen_args); \
	scope bool prefix ## check(csnip_pp_list_##gen_args);

/** Generator macro to define heap functions.
 *
 *  @param	scope
 *		function scope
 *
 *  @param	prefix
 *		function name prefixes
 *
 *  @param	gen_args
 *		argument list, either of the form args(...) or noargs().
 *
 *  @param	u, v
 *		dummy variables
 *
 *  @param	au_lessthan_av
 *		comparator expression
 *
 *  @param	swap_au_av
 *		entry swapping statement
 *
 *  @param	K
 *		heap arity (e.g., 2 for binary heaps)
 *
 *  @param	N
 *		heap size
 *
 */
#define CSNIP_HEAP_DEF_FUNCS(scope, prefix, gen_args, \
	u, v, au_lessthan_av, swap_au_av, K, N) \
	scope void prefix ## sift_up(csnip_pp_prepend_##gen_args \
				size_t i) \
	{ \
		csnip_heap_SiftUp(u, v, \
			au_lessthan_av, swap_au_av, \
			K, N, i); \
	} \
	\
	scope void prefix ## sift_down(csnip_pp_prepend_##gen_args \
				size_t i) \
	{ \
		csnip_heap_SiftDown(u, v, \
			au_lessthan_av, swap_au_av, \
			K, N, i); \
	} \
	\
	scope void prefix ## heapify(csnip_pp_list_##gen_args) \
	{ \
		csnip_heap_Heapify(u, v, \
			au_lessthan_av, swap_au_av, \
			K, N); \
	} \
	scope bool prefix ## check(csnip_pp_list_##gen_args) \
	{ \
		bool csnip_heap_ret; \
		csnip_heap_Check(u, v, \
			au_lessthan_av, swap_au_av, \
			K, N, csnip_heap_ret); \
		return csnip_heap_ret; \
	} \

/** @} */

#endif /* CSNIP_HEAP_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_HEAP_HAVE_SHORT_NAMES)
#define heap_SiftUp		csnip_heap_SiftUp
#define heap_SiftDown		csnip_heap_SiftDown
#define heap_Heapify		csnip_heap_Heapify
#define heap_Check		csnip_heap_Check
#define CSNIP_HEAP_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_HEAP_HAVE_SHORT_NAMES */
