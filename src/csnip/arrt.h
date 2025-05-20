#ifndef CSNIP_ARRT_H
#define CSNIP_ARRT_H

/**	@file arrt.h
 *	@brief				Dynamic arrays container type
 *	@defgroup arrt			Dynamic arrays container type
 *	@{
 *
 *	@brief Dynamic arrays container type.
 *
 *	Defines a container type for dynamic arrays, and wraps the
 *	csnip_arr_* macros so that the container is used.  Often, this
 *	is simpler than using csnip's lower level (but more flexible)
 *	arr interface.  Another advantage over using arr directly is
 *	that it standardizes the members of the array.  For a type T,
 *	the container type is of the form:
 *	```
 *	struct {
 *		T* a;		// array itself
 *		size_t n;	// size of the array in members
 *		size_t cap;	// capacity of the array
 *	};
 *	```
 *	Having the same standardized form in many places reduces
 *	cognitive load.
 */

#include <stddef.h>

#include <csnip/arr.h>

/**	Initialize an array.
 *
 *	arrt version of @sa csnip_arr_Init()
 */
#define csnip_arrt_Init(arrp, initial_cap, err) \
	csnip_arr_init((arrp)->a, (arrp)->n, (arrp)->cap, (initial_cap), (err))

/**	Reserve space for members to be added.
 *
 *	arrt version of @sa csnip_arr_Reserve()
 */
#define csnip_arrt_Reserve(arrp, least_cap, err) \
	csnip_arr_Reserve((arrp)->a, (arrp)->n, (arrp)->cap, (least_cap), (err))

/**	Append a new value at the end of the array.
 *
 *	arrt version of @sa csnip_arr_Push()
 */
#define csnip_arrt_Push(arrp, value, err) \
	csnip_arr_Push((arrp)->a, (arrp)->n, (arrp)->cap, (value), (err))

/**	Delete the value at the end of the array.
 *
 *	arrt version of @sa csnip_arr_Pop()
 */
#define csnip_arrt_Pop(arrp, err) \
	csnip_arr_Pop(((arrp)->a, (arrp)->n, (arrp)->cap, (err))

/**	Insert a new member at a given position in the array.
 *
 *	arrt version of @sa csnip_arr_InsertAt()
 */
#define csnip_arrt_InsertAt(arrp, index, val, err) \
	csnip_arr_InsertAt((arrp)->a, (arrp)->n, (arrp)->cap, \
		(index), (val), (err))

/**	Remove an array member at a given index.
 *
 *	arrt version of @sa csnip_arr_DeleteAt()
 */
#define csnip_arrt_DeleteAt(arrp, index, err) \
	csnip_arr_DeleteAt((arrp)->a, (arrp)->n, (arrp)->cap, (index), (err))

/**	Free backend storage and reduce size to 0.
 *
 *	arrt version of @sa csnip_arr_Deinit()
 */
#define csnip_arrt_Deinit(arrp) \
	csnip_arr_Free((arrp)->a, (arrp)->n, (arrp)->cap)

/**	Define an array type.
 *
 *	Creates a struct typedef for an array type, consisting of the
 *	members `a` (the pointer to the first member), `n`, the array
 *	size, and `cap`, the array capacity.
 *
 *	@param	arr_type
 *		name of the type to assign.
 *
 *	@param	elem_type
 *		type of the elements.
 */
#define CSNIP_ARRT_DEF_TYPE(arr_type, elem_type) \
	typedef struct arr_type ## _s { \
		elem_type* a; \
		size_t n; \
		size_t cap; \
	} arr_type;

/**	Declare the array type.
 *
 *	As @sa CSNIP_ARRT_DEF_TYPE, but don't define the struct, only
 *	declare it as an opaque type.
 */
#define CSNIP_ARRT_DECL_TYPE(arr_type, elem_type) \
	struct arr_type ## _s; \
	typedef struct arr_type ## _s arr_type;

/**	Declare the array functions. */
#define CSNIP_ARRT_DECL_FUNCS(scope, prefix, arr_type, val_type) \
	CSNIP_ARR_DECL_FUNCS(scope, prefix, val_type, \
			args(arr_type*, int*))

/**	Define the array functions.
 *
 *	The functions defined are the same as in the lower level arr
 *	module, @sa CSNIP_ARR_DEF_FUNCS.
 */
#define CSNIP_ARRT_DEF_FUNCS(scope, prefix, arr_type, val_type) \
	CSNIP_ARR_DEF_FUNCS(scope, prefix, val_type, \
			args(arr_type* A, int* err), \
			A->a, A->n, A->cap, *err)

/** @} */

#endif /* CSNIP_ARRT_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_ARRT_HAVE_SHORT_NAMES)
#define arrt_Init		csnip_arr_Init
#define arrt_Reserve		csnip_arr_Reserve
#define arrt_Push		csnip_arr_Push
#define arrt_Pop		csnip_arr_Pop
#define arrt_InsertAt		csnip_arr_InsertAt
#define arrt_DeleteAt		csnip_arr_DeleteAt
#define arrt_Free		csnip_arr_Free
#define CSNIP_ARR_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_ARRT_HAVE_SHORT_NAMES */
