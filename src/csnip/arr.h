#ifndef CSNIP_ARR_H
#define CSNIP_ARR_H

/**	@file arr.h
 *	@brief			Dynamic arrays
 *	@defgroup arr		Dynamic arrays
 *	@{
 *
 *	@brief Dynamically allocated resizable arrays.
 *
 *	A csnip dynamic array is represented as a triple (a, n, cap),
 *	where "a" is a pointer to the array members, n is an integer
 *	value containing the current size of the dynamic array (in
 *	number of members), and cap is another integer value containing
 *	the current capacity of the array, i.e., the number of members
 *	that can be stored without reallocating the array.
 *
 *	The array macros assume the triple is given as lvalues, and will
 *	suitably modify them.  The pointer to the array must be either a
 *	NULL pointer (allowable for empty arrays), or a pointer suitable
 *	for modification with realloc or free.
 *
 *	It is of course OK to modify the array directly without using
 *	the csnip_arr_* macros.  No "SetAt" and "GetAt" methods are
 *	provided, since the user can trivially access the array members
 *	like a C array.
 */

#include <stddef.h>

#include <csnip/err.h>
#include <csnip/mem.h>
#include <csnip/util.h>
#include <csnip/preproc.h>

/**	Initialize an array.
 *
 *	An empty array of the given initial capacity is allocated.  No
 *	memory is allocated if the initial_cap value provided is 0.
 */
#define csnip_arr_Init(a, n, cap, initial_cap, err) \
		do { \
			(n) = 0; \
			if (((cap) = (initial_cap)) > 0) { \
				csnip_mem_Alloc(initial_cap, a, err); \
			} else { \
				(a) = NULL; \
			} \
		} while(0)

/**	Reserve space for members to be added.
 *
 *	Increase the capacity of the (a, n, cap) to at least least_cap,
 *	possibly reallocating the array if necessary.
 *
 *	Reallocation is designed to be efficient even if the provided
 *	increments are small.  The csnip_arr_Reserve() method achieves
 *	this by increasing the capacity often to more than the least_cap
 *	value that was requested.
 *
 *	It is possible to shrink the reserved size of an array by
 *	calling csnip_arr_Reserve, but not to shrink the array itself.
 *	That is, requests for least_cap < n are not honoured.
 */
#define csnip_arr_Reserve(a, n, cap, least_cap, err) \
	csnip_arr__Reserve((a), (n), (cap), (least_cap), (err), \
				csnip__i, csnip__err2)

/** @cond */
#define csnip_arr__Reserve(a, n, cap, least_cap, err,		i, err2) \
	do { \
		size_t i = csnip_next_pow_of_2(csnip_Max(least_cap, n)); \
		if(i != (size_t)cap) { \
			int err2 = 0; \
			csnip_mem_Realloc(i, a, err2); \
			if (err2) { \
				csnip_err_Raise(err2, err); \
				break; \
			} \
			cap = i; \
		} \
	} while(0)
/** @endcond */

/**	Append a new value at the end of the array.
 *
 *	Complexity: amortized O(1).
 */
#define csnip_arr_Push(a, n, cap, value, err) \
		do { \
			int csnip_err = 0; \
			csnip_arr_Reserve(a, n, cap, (n) + 1, csnip_err); \
			if (csnip_err) { \
				csnip_err_Raise(csnip_err, err); \
				break; \
			} \
			(a)[(n)++] = (value); \
		} while(0)

/**	Delete the value at the end of the array.
 *
 *	This removes the last element of the array.  The array capacity
 *	is not reduced accordingly.
 *
 *	Complexity: O(1)
 */
#define csnip_arr_Pop(a, n, cap, err) \
		do { \
			if ((n) <= 0) { \
				csnip_err_Raise(csnip_err_UNDERFLOW, err); \
				break; \
			} \
			--(n); \
		} while(0);


/**	Insert a new member at a given position in the array.
 *
 *	The members currently at the given position and after are moved
 *	back one index.
 *
 *	Complexity:  n - index element moves.
 */
#define csnip_arr_InsertAt(a, n, cap, index, val, err) \
		do { \
			int csnip_err = 0; \
			csnip_arr_Reserve(a, n, cap, (n) + 1, csnip_err); \
			if (csnip_err) { \
				csnip_err_Raise(csnip_err, err); \
				break; \
			} \
			for (int csnip_i = (n); csnip_i > (index); --csnip_i)\
			{ \
				(a)[csnip_i] = (a)[csnip_i - 1]; \
			} \
			(a)[(index)] = (val); \
		} while(0)

/**	Remove an array member at a given index.
 *
 *	Array members appearing behind the given index are moved forward
 *	one position.
 *
 *	Complexity:	n - index element moves.
 */
#define csnip_arr_DeleteAt(a, n, cap, index, err) \
		do { \
			int csnip_i; \
			--(n); \
			for (csnip_i = (index); csnip_i < (n); ++csnip_i) \
			{ \
				(a)[csnip_i] = (a)[csnip_i + 1]; \
			} \
		} while(0)

/**	Delete an array.
 *
 *	This function deletes the memory associated with an array.  The
 *	result is an array of size 0 with no allocated memory
 *	associated, i.e., it is like a freshly allocated array of
 *	capacity 0.
 */
#define csnip_arr_Free(a, n, cap) \
		do { \
			csnip_mem_Free(a); \
			(n) = 0; \
			(cap) = 0; \
		} while(0)

/**	Declare array managment functions.
 *
 *	This macro declares array manipulation functions corresponding
 *	to such functions defined with CSNIP_ARR_DEF_FUNCS.  See
 *	CSNIP_ARR_DEF_FUNCS for details.
 *
 *	@param	scope
 *		declaration scope of the functions;  leave empty for
 *		global scope, "static" for file scope, etc.
 *
 *	@param	prefix
 *		prefix to use for all function names.
 *
 *	@param	val_type
 *		the type used for array values.
 *
 *	@param	gen_args
 *		either a list of arguments specified as args(list) to
 *		add to all functions, or noargs() if no such arguments
 *		are to be used.
 */
#define CSNIP_ARR_DECL_FUNCS(scope, prefix, val_type, gen_args) \
		scope void prefix ## init(csnip_pp_prepend_##gen_args \
					size_t cs); \
		scope void prefix ## reserve(csnip_pp_prepend_##gen_args \
					size_t least_cap); \
		scope void prefix ## push(csnip_pp_prepend_##gen_args \
					val_type v); \
		scope void prefix ## pop(csnip_pp_list_##gen_args); \
		scope void prefix ## insert_at(csnip_pp_prepend_##gen_args \
					size_t i, val_type v); \
		scope void prefix ## delete_at(csnip_pp_prepend_##gen_args \
					size_t i); \
		scope void prefix ## free(csnip_pp_list_##gen_args);

/**	Define dynamic array managment functions.
 *
 *	@param	scope
 *		declaration scope of the functions;  leave empty for
 *		global scope, "static" for file scope, etc.
 *
 *	@param	prefix
 *		prefix to use for all function names.
 *
 *	@param	val_type
 *		the type used for array values.
 *
 *	@param	gen_args
 *		either a list of arguments specified as args(list) to
 *		add to all functions, or noargs() if no such arguments
 *		are to be used.
 *
 *	@param	a, n, cap, err
 *		those are the expressions as passed to the csnip_arr_*
 *		macros used to define the functions; they can be
 *		functions of the arguments as specified with @a gen_args .
 */
#define CSNIP_ARR_DEF_FUNCS(scope, prefix, val_type, gen_args, a, n, cap, err) \
		scope void prefix ## init(csnip_pp_prepend_##gen_args \
					size_t cs) \
		{ \
			csnip_arr_Init(a, n, cap, cs, err); \
		} \
		\
		scope void prefix ## reserve(csnip_pp_prepend_##gen_args \
					size_t least_cap) \
		{ \
			csnip_arr_Reserve(a, n, cap, least_cap, err); \
		} \
		\
		scope void prefix ## push(csnip_pp_prepend_##gen_args \
					val_type v) \
		{ \
			csnip_arr_Push(a, n, cap, v, err); \
		} \
		\
		scope void prefix ## pop(csnip_pp_list_##gen_args) \
		{ \
			csnip_arr_Pop(a, n, cap, err); \
		} \
		\
		scope void prefix ## insert_at(csnip_pp_prepend_##gen_args \
			size_t i, val_type v) \
		{ \
			csnip_arr_InsertAt(a, n, cap, i, v, err); \
		} \
		\
		scope void prefix ## delete_at(csnip_pp_prepend_##gen_args \
			size_t i) \
		{ \
			csnip_arr_DeleteAt(a, n, cap, i, err); \
		} \
		scope void prefix ## free(csnip_pp_list_##gen_args) \
		{ \
			csnip_arr_Free(a, n, cap); \
		}

#ifdef CSNIP_SHORT_NAMES
#define arr_Init		csnip_arr_Init
#define arr_Reserve		csnip_arr_Reserve
#define arr_Push		csnip_arr_Push
#define arr_Pop			csnip_arr_Pop
#define arr_InsertAt		csnip_arr_InsertAt
#define arr_DeleteAt		csnip_arr_DeleteAt
#define arr_Free		csnip_arr_Free
#endif

/** @} */

#endif /* CSNIP_ARR_H */
