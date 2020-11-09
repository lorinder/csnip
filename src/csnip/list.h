#ifndef CSNIP_LIST_H
#define CSNIP_LIST_H

/**	@file list.h
 *	@brief			Linked Lists
 *	@defgroup list		Linked Lists
 *	@{
 *
 *	@brief Macros to manipulate linked lists
 *
 *	These macros implement intrusive linked lists.  Two variants are
 *	provided:  slist for singly linked lists, and dlist for doubly
 *	linked lists.
 *
 *	Linked lists can be used as stacks and deques for example, but
 *	they usually not the most efficient implementation of those data
 *	structures, due to their poor locality.  However, they have some
 *	nice properties that make them very useful:  Pointers to list
 *	elements are not invalidated through manipulation of lists, and
 *	it's easy to move list elements between lists.  They are often
 *	the most elegant solution when data records need to be managed
 *	in multiple disjoint lists, e.g. the differnt lists holding
 *	records in states.
 *
 *	Lists are fairly simple data structures, and this implementation
 *	tries to preserve the spirit of simplicity:  it provides only
 *	the basic list operations themselves, without e.g. element
 *	allocation and deallocation.  Most list operations therefore
 *	cannot fail (though misuse can corrupt the data structures).
 *	csnip_err is therefore not used in this module.
 */

#include <assert.h>
#include <stddef.h>

#include <csnip/preproc.h>

/**	@defgroup dlist		Doubly Linked Lists
 *	@{
 *
 *	@brief Macros for doubly linked lists.
 *
 *	Doubly linked lists (dlists) have pointers to both the previous
 *	and the next entry in each entry.  Thus, they can be traversed
 *	in both directions.  Among other things, they provide all the
 *	operations of a deque.
 *
 *	All the dlist macros take the 4 initial arguments @a head, @a
 *	tail, @a mprev and @a mnext.  @a head is the lvalue pointing to
 *	the head of the list; may be modified by the operations; @a tail
 *	is the lvalue pointing to the tail of the list, again modifiable
 *	by the list operations. @a mprev and @a mnext are the names of
 *	the struct fields containing the pointer to the previous and
 *	next elements, respectively.
 *
 *	Simple dlist example (from examples/dlist.c):
 *
 *	\include dlist.c
 */

/**	Initialize a dlist. */
#define csnip_dlist_Init(head, tail, mprev, mnext) \
	do { \
		(head) = (tail) = NULL; \
	} while (0)

/**	Push an element to the head of a dlist. */
#define csnip_dlist_PushHead(head, tail, mprev, mnext, el) \
	do { \
		assert((el) != NULL); \
		if ((head) != NULL) { \
			(head)->mprev = (el); \
		} else { \
			assert((tail) == NULL); \
			(tail) = (el); \
		} \
		(el)->mnext = (head); \
		(el)->mprev = NULL; \
		(head) = (el); \
	} while (0)

/**	Pop the head of a dlist.
 *
 *	Underflowing the list results in an assertion failure;
 *	therefore, only pop the list if it's known that the list is
 *	non-empty.
 */
#define csnip_dlist_PopHead(head, tail, mprev, mnext) \
	do { \
		assert((head) != NULL); \
		(head) = (head)->mnext; \
		if ((head) != NULL) { \
			(head)->mprev = NULL; \
		} else { \
			(tail) = NULL; \
		} \
	} while (0)

/**	Push an element to the tail of a dlist. */
#define csnip_dlist_PushTail(head, tail, mprev, mnext, el) \
	do { \
		if ((head) == NULL) { \
			assert((tail) == NULL); \
			(head) = (el); \
		} else { \
			(tail)->mnext = (el); \
		} \
		(el)->mprev = (tail); \
		(el)->mnext = NULL; \
		(tail) = el; \
	} while (0)

/**	Pop from the tail of a dlist.
 *
 *	Underflowing the list results in an assert failure.
 */
#define csnip_dlist_PopTail(head, tail, mprev, mnext) \
	do { \
		assert((tail) != NULL); \
		(tail) = (tail)->mprev; \
		if ((tail) != NULL) { \
			(tail)->mnext = NULL; \
		} else { \
			(head) = NULL; \
		} \
	} while (0)

/**	Insert an element after the end of the given location.
 *
 *	@param	head, tail, mprev, mnext
 *		list description.
 *
 *	@param	loc
 *		point after which to insert (pointer to a list element).
 *
 *	@param	el
 *		element to insert.
 */
#define csnip_dlist_InsertAfter(head, tail, mprev, mnext, loc, el) \
	do { \
		(el)->mprev = (loc); \
		(el)->mnext = (loc)->mnext; \
		if ((loc)->mnext != NULL) { \
			(loc)->mnext->mprev = (el); \
		} else { \
			assert((loc) == (tail)); \
			(tail) = (el); \
		} \
		(loc)->mnext = (el); \
	} while (0)

/**	Insert an element before @a loc.
 *
 *	@param	head, tail, mprev, mnext
 *		list description
 *
 *	@param	loc
 *		point before which to insert (pointer to a list
 *		element).
 *
 *	@param	el
 *		element to insert (pointer to element to add).
 */
#define csnip_dlist_InsertBefore(head, tail, mprev, mnext, loc, el) \
	do { \
		(el)->mnext = (loc); \
		(el)->mprev = (loc)->mprev; \
		if ((loc)->mprev != NULL) { \
			(loc)->mprev->mnext = (el); \
		} else { \
			assert((loc) == (head)); \
			(head) = (el); \
		} \
		(loc)->mprev = (el); \
	} while (0)

/**	Remove an element from the list.
 *
 *	This removes the element from the list;  the element itself
 *	still exists after the call, and its @a prev and @a next
 *	pointers are not updated; but it's no longer part of the list.
 */
#define csnip_dlist_Remove(head, tail, mprev, mnext, el) \
	do { \
		if ((el)->mprev != NULL) { \
			(el)->mprev->mnext = (el)->mnext; \
		} else { \
			assert((el) == (head)); \
			(head) = (el)->mnext; \
		} \
		if ((el)->mnext != NULL) { \
			(el)->mnext->mprev = (el)->mprev; \
		} else { \
			assert((el) == (tail)); \
			(tail) = (el)->mprev; \
		} \
	} while (0)

/**	Declare dlist functions.
 *
 *	This macro declares a set of dlist functions that wrap the
 *	csnip_dlist_* macros; the functions can be defined with
 *	CSNIP_DLIST_DEF_FUNCS().  For a description of the macro
 *	arguments, see CSNIP_DLIST_DEF_FUNCS().
 */
#define CSNIP_DLIST_DECL_FUNCS(scope, prefix, entry_ptr_type, gen_args) \
	scope void prefix ## init(csnip_pp_list_##gen_args); \
	scope void prefix ## push_head(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el); \
	scope void prefix ## pop_head(csnip_pp_list_##gen_args); \
	scope void prefix ## push_tail(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el); \
	scope void prefix ## pop_tail(csnip_pp_list_##gen_args); \
	scope void prefix ## insert_after(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el); \
	scope void prefix ## insert_before(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el); \
	scope void prefix ## remove(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el); \
	\
	/* functions without corresponding macros */ \
	scope entry_ptr_type prefix ## head(csnip_pp_list_##gen_args); \
	scope entry_ptr_type prefix ## tail(csnip_pp_list_##gen_args); \
	scope entry_ptr_type prefix ## prev(entry_ptr_type csnip_el); \
	scope entry_ptr_type prefix ## next(entry_ptr_type csnip_el); \
	scope char prefix ## is_empty(csnip_pp_list_##gen_args); \

/**	Define a set of dlist functions.
 *
 *	@param	scope
 *		the scope of the function declaration.
 *
 *	@param	prefix
 *		the prefix for the function names.
 *
 *	@param	entry_ptr_type
 *		pointer type to a list entry.
 *
 *	@param	gen_args
 *		generic arguments to specify the list, of the form
 *		args(...) or noargs().
 *
 *	@param	phead
 *		lvalue pointer to the list head, can be expressed in
 *		terms of the gen_args arguements
 *
 *	@param	ptail
 *		Like phead, but for the list tail.
 *
 *	@param	mprev, mnext
 *		The struct entry names for pointers to the previous,
 *		respectively next entry.
 *
 *	The list functions defined, are (without the prefix):
 *
 *	* void init(genargs)
 *	* void push_head(genargs, entry_ptr_type element)
 *	* void pop_head(genargs)
 *	* void push_tail(genargs, entry_ptr_type element)
 *	* void pop_tail(genargs)
 *	* void insert_after(genargs, entry_ptr_type loc, entry_ptr_type element)
 *	* void insert_before(genargs, entry_ptr_type loc, entry_ptr_type element)
 *	* void remove(genargs, entry_ptr_type element)
 *	* entry_ptr_type head(genargs)
 *	* entry_ptr_type tail(genargs)
 *	* entry_ptr_type prev(entry_ptr_type el)
 *	* entry_ptr_type next(entry_ptr_type el)
 *	* char is_empty(genargs)
 */
#define CSNIP_DLIST_DEF_FUNCS(scope, prefix, entry_ptr_type, gen_args, \
	phead, ptail, mprev, mnext) \
	scope void prefix ## init(csnip_pp_list_##gen_args) \
	{ \
		csnip_dlist_Init(phead, ptail, mprev, mnext); \
	} \
	scope void prefix ## push_head(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el) \
	{ \
		csnip_dlist_PushHead(phead, ptail, mprev, mnext, csnip_el); \
	} \
	scope void prefix ## pop_head(csnip_pp_list_##gen_args) \
	{ \
		csnip_dlist_PopHead(phead, ptail, mprev, mnext); \
	} \
	scope void prefix ## push_tail(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el) \
	{ \
		csnip_dlist_PushTail(phead, ptail, mprev, mnext, csnip_el); \
	} \
	scope void prefix ## pop_tail(csnip_pp_list_##gen_args) \
	{ \
		csnip_dlist_PopTail(phead, ptail, mprev, mnext); \
	} \
	scope void prefix ## insert_after(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el) \
	{ \
		csnip_dlist_InsertAfter(phead, ptail, mprev, mnext, \
		  csnip_loc, csnip_el); \
	} \
	scope void prefix ## insert_before(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el) \
	{ \
		csnip_dlist_InsertBefore(phead, ptail, mprev, mnext, \
		  csnip_loc, csnip_el); \
	} \
	scope void prefix ## remove(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el) \
	{ \
		csnip_dlist_Remove(phead, ptail, mprev, mnext, csnip_el); \
	} \
	\
	/* functions without corresponding macros */ \
	scope entry_ptr_type prefix ## head(csnip_pp_list_##gen_args) { \
		return phead; \
	} \
	scope entry_ptr_type prefix ## tail(csnip_pp_list_##gen_args) { \
		return ptail; \
	} \
	scope entry_ptr_type prefix ## prev(entry_ptr_type csnip_el) { \
		return csnip_el->mprev; \
	} \
	scope entry_ptr_type prefix ## next(entry_ptr_type csnip_el) { \
		return csnip_el->mnext; \
	} \
	scope char prefix ## is_empty(csnip_pp_list_##gen_args) { \
		return phead == NULL; \
	}

/** @} */

/** @defgroup slist		Singly Linked Lists with tail pointers
 *  @{
 */

/**	Initialize an slist. */
#define csnip_slist_Init(head, tail, mnext) \
	do { \
		(head) = (tail) = NULL; \
	} while (0)

/**	Push an element to the head of an slist. */
#define csnip_slist_PushHead(head, tail, mnext, el) \
	do { \
		(el)->mnext = (head); \
		if ((tail) == NULL) { \
			assert((head) == NULL); \
			(tail) = (el); \
		} \
		(head) = (el); \
	} while (0)

/**	Pop the head of an slist.
 *
 *	Underflowing the list results in an assertion failure;
 *	therefore, only pop the list if it's known that the list is
 *	non-empty.
 */
#define csnip_slist_PopHead(head, tail, mnext) \
	do { \
		assert((head) != NULL); \
		if ((tail) == (head)) { \
			(head) = (tail) = NULL; \
		} else { \
			(head) = (head)->mnext; \
		} \
	} while (0)

/**	Push an element to the tail of an slist. */
#define csnip_slist_PushTail(head, tail, mnext, el) \
	do { \
		if ((head) == NULL) { \
			assert((tail) == NULL); \
			(head) = (tail) = (el); \
		} else { \
			(tail)->mnext = (el); \
			(tail) = (el); \
		} \
		(el)->mnext = NULL; \
	} while (0)

/**	Insert an element after the end of the given location.
 *
 *	@param	head, tail, mnext
 *		list description.
 *
 *	@param	loc
 *		point after which to insert (pointer to a list element).
 *
 *	@param	el
 *		element to insert.
 */
#define csnip_slist_InsertAfter(head, tail, mnext, loc, el) \
	do { \
		(el)->mnext = (loc)->mnext; \
		(loc)->mnext = (el); \
		if ((el)->mnext == NULL) { \
			(tail) = (el); \
		} \
	} while (0)

/**	Declare slist functions.
 *
 *	This macro declares a set of slist functions that wrap the
 *	csnip_slist_* macros; the functions can be defined with
 *	CSNIP_SLIST_DEF_FUNCS().  For a description of the macro
 *	arguments, see CSNIP_SLIST_DEF_FUNCS().
 *
 */
#define CSNIP_SLIST_DECL_FUNCS(scope, prefix, entry_ptr_type, gen_args) \
	scope void prefix ## init(csnip_pp_list_##gen_args); \
	scope void prefix ## push_head(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el); \
	scope void prefix ## pop_head(csnip_pp_list_##gen_args); \
	scope void prefix ## push_tail(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el); \
	scope void prefix ## insert_after(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el); \
	\
	/* functions without corresponding macros */ \
	scope entry_ptr_type prefix ## head(csnip_pp_list_##gen_args); \
	scope entry_ptr_type prefix ## tail(csnip_pp_list_##gen_args); \
	scope entry_ptr_type prefix ## next(entry_ptr_type csnip_el); \
	scope char prefix ## is_empty(csnip_pp_list_##gen_args); \

/**	Define a set of slist functions.
 *
 *	@param	scope
 *		the scope of the function declaration.
 *
 *	@param	prefix
 *		the prefix for the function names.
 *
 *	@param	entry_ptr_type
 *		pointer type to a list entry.
 *
 *	@param	gen_args
 *		generic arguments to specify the list, of the form
 *		args(...) or noargs().
 *
 *	@param	phead
 *		lvalue pointer to the list head, can be expressed in
 *		terms of the gen_args arguements
 *
 *	@param	ptail
 *		Like phead, but for the list tail.
 *
 *	@param	mnext
 *		The struct entry name for pointers to the next entry.
 *
 *	The list functions defined, are (without the prefix):
 *
 *	* void init(genargs)
 *	* void push_head(genargs, entry_ptr_type element)
 *	* void pop_head(genargs)
 *	* void push_tail(genargs, entry_ptr_type element)
 *	* void insert_after(genargs, entry_ptr_type loc, entry_ptr_type element)
 *	* entry_ptr_type head(genargs)
 *	* entry_ptr_type tail(genargs)
 *	* entry_ptr_type next(entry_ptr_type el)
 *	* char is_empty(genargs)
 */
#define CSNIP_SLIST_DEF_FUNCS(scope, prefix, entry_ptr_type, gen_args, \
	phead, ptail, mnext) \
	scope void prefix ## init(csnip_pp_list_##gen_args) \
	{ \
		csnip_slist_Init(phead, ptail, mnext); \
	} \
	scope void prefix ## push_head(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el) \
	{ \
		csnip_slist_PushHead(phead, ptail, mnext, csnip_el); \
	} \
	scope void prefix ## pop_head(csnip_pp_list_##gen_args) \
	{ \
		csnip_slist_PopHead(phead, ptail, mnext); \
	} \
	scope void prefix ## push_tail(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_el) \
	{ \
		csnip_slist_PushTail(phead, ptail, mnext, csnip_el); \
	} \
	scope void prefix ## insert_after(csnip_pp_prepend_##gen_args \
					entry_ptr_type csnip_loc, \
					entry_ptr_type csnip_el) \
	{ \
		csnip_slist_InsertAfter(phead, ptail, mnext, \
		  csnip_loc, csnip_el); \
	} \
	\
	/* functions without corresponding macros */ \
	scope entry_ptr_type prefix ## head(csnip_pp_list_##gen_args) { \
		return phead; \
	} \
	scope entry_ptr_type prefix ## tail(csnip_pp_list_##gen_args) { \
		return ptail; \
	} \
	scope entry_ptr_type prefix ## next(entry_ptr_type csnip_el) { \
		return csnip_el->mnext; \
	} \
	scope char prefix ## is_empty(csnip_pp_list_##gen_args) { \
		return phead == NULL; \
	}

/** @}
    @}
 */

#endif /* CSNIP_LIST_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_LIST_HAVE_SHORT_NAMES)
#define	dlist_Init		csnip_dlist_Init
#define dlist_IsEmpty		csnip_dlist_IsEmpty
#define dlist_PushHead		csnip_dlist_PushHead
#define dlist_PopHead		csnip_dlist_PopHead
#define dlist_PushTail		csnip_dlist_PushTail
#define dlist_PopTail		csnip_dlist_PopTail
#define dlist_InsertAfter	csnip_dlist_InsertAfter
#define dlist_InsertBefore	csnip_dlist_InsertBefore
#define dlist_Remove		csnip_dlist_Remove
#define slist_Init		csnip_slist_Init
#define slist_PushHead		csnip_slist_PushHead
#define slist_PopHead		csnip_slist_PopHead
#define slist_PushTail		csnip_slist_PushTail
#define slist_InsertAfter	csnip_slist_InsertAfter
#define CSNIP_LIST_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_LIST_HAVE_SHORT_NAMES */
