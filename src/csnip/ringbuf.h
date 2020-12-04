#ifndef CSNIP_RINGBUF_H
#define CSNIP_RINGBUF_H

/**	@file ringbuf.h
 *	@brief				Ring buffers
 *	@defgroup ringbuf		Ring buffers
 *	@{
 *
 *	@brief Ring buffer implementation
 *
 *	This module provides a simple ring buffer.  That is to say, it
 *	provides two indices, head and tail, that wrap around at the
 *	end of the array.  The following illustrates a ring buffer with
 *	elements X, Y and Z from head to tail.
 *	@verbatim
		,---------------------------.
		|  ...  | X | Y | Z |  ...  |
		`---------------------------'
		          ^       ^          ^
		          head    tail       N (capacity)
	@endverbatim
 *      The ring buffer can wrap around at the edges, thus the following
 *      picture gives a ring buffer with the same entries in the same
 *      order as above:
 *	@verbatim
		,---------------------------.
		| Y | Z |    ...        | X |
		`---------------------------'
		      ^                   ^  ^
		      tail             head  N (capacity)
	@endverbatim
 *	Ring buffers are represented with three integers: @a head, @a
 *	len and @a N:  @a head is the location of the head of the buffer
 *	in the backing array, @a len is the number of members, and @a N
 *	is the size of the backing array.  Thus 0 <= @a head < @a N and
 *	0 <= @a len <= @a N .  The value @a N is read only.
 *
 *	Elements can be added (pushed) or removed (popped) from either
 *	end, head or tail.  Thus, ring buffers provide essentially the
 *	functionality of a deque of fixed capacity.  The fixed size of
 *	the backing array has an important advantage: inserted elements
 *	will have a fixed location in the backing array for their entire
 *	lifetime.  This allows updating specific elements of the
 *	ring buffer after they were added.
 *
 *	There are three families of macros or functions provided, the
 *	index macros, including csnip_ringbuf_Init(), which operate
 *	exclusively on backing array indices; those generally have names
 *	ending in Idx.  Then there are the index arithmetic macros which
 *	perform simple index arithmetic mod N in a safe way; they have
 *	"Wrap" in their name.  Finally there are the value macros, which
 *	do access the backing array as well.
 *
 *	*Comment.*  The alert user may wonder whether it is really
 *	neccessary to implement the Idx macros as macros, or whether
 *	they should have been functions; they don't really need
 *	template-esque properties.  It is the case, however, that it's
 *	fashionable, these days, to use size_t to loop over array
 *	indices.  I personally think that doing so is unwise and there
 *	are important reasons to stick to int in almost all cases.
 *
 *	Using macros allows the use of size_t if it is preferred or
 *	necessary, but allows the use of int in all other cases.
 *
 *	Why do I not like size_t as array index?
 *
 *	1. It is ugly, though not quite as ugly as
 *	std::vector<mytype>::size_type.
 *
 *	2. It has interesting problems due to its unsigned nature,
 *	chiefly the following doesn't work: \code
		for (size_t i = N - 1; i >= 0; --i) { ... }
	\endcode
 *
 *	3. I've seen it affect performance (program speed) quite
 *	dramatically in some cases.
 *
 *	In the vast majority of cases, it is known that int will be
 *	large enough as array index, and for those cases, I recommend
 *	sticking to it.
 */

#include <stddef.h>

#include <csnip/err.h>
#include <csnip/preproc.h>

/** Initialize an empty ring buffer.
 *
 *  Sets the head and length of the ring buffer to 0.  Note that the
 *  capacity value N is not initialized.  (Not all ring buffers allow
 *  for the N parameter to be set, e.g. it may be a compile time
 *  constant.)
 */
#define csnip_ringbuf_Init(head, len, N) \
	do { \
		(head) = 0; \
		(len) = 0; \
	} while (0)

/** Compute the location of the head in the backing array.
 *
 *  This macro computes the array index of the current head in the
 *  backing array.  If the ring buffer is empty, csnip_err_UNDERFLOW is
 *  raised.
 *
 *  Thus, save for checking whether the ring buffer is empty, this macro
 *  simply sets @a ret to @a head.
 *
 *  @param	head
 *		index to head element in ring buffer.
 *
 *  @param	len
 *		amount of data filled into ring buffer.
 *
 *  @param	N
 *		ring buffer array size.
 *
 *  @param	ret
 *		l-value to assign the computed index.
 *
 *  @param	err
 *		error return.
 */
#define csnip_ringbuf_GetHeadIdx(head, len, N, ret, err) \
	do { \
		if ((len) == 0) { \
			csnip_err_Raise(csnip_err_UNDERFLOW, err); \
			break; \
		} \
		(ret) = (head); \
	} while (0)

/** Compute the location of the tail in the backing array.
 *
 *  This macro computes the array index of the current tail in the
 *  backing array.  If the ring buffer is empty, csnip_err_UNDERFLOW is
 *  raised.
 *
 *  Thus, for non-empty ring buffers, this computes (head + len - 1) mod N
 *  in the range 0, ..., N - 1.
 */
#define csnip_ringbuf_GetTailIdx(head, len, N, ret, err) \
	csnip_ringbuf__GetTailIdx((head), (len), (N), (ret), (err))

/** @cond */
#define csnip_ringbuf__GetTailIdx(head, len, N, ret, err) \
	do { \
		if (len == 0) { \
			csnip_err_Raise(csnip_err_UNDERFLOW, err); \
			break; \
		} \
		ret = csnip_ringbuf__AddWrap(N, (len - 1), head); \
	} while (0)
/** @endcond */

/** Add an element at the head.
 *
 *  This changes @a head and @a len as if an element had been added to
 *  the buffer at the head.  Since this is an index function, it does
 *  not actually add any element, only update the pointers/indices.  In
 *  other works, the macro "makes space" for an element that can be
 *  added by assigning to the new head index in the backing array.
 *
 *  Possible errors include csnip_err_NOMEM, for when the ring buffer is
 *  full.
 */
#define csnip_ringbuf_PushHeadIdx(head, len, N, err) \
	csnip_ringbuf__PushHeadIdx((head), (len), (N), (err))

/** @cond */
#define csnip_ringbuf__PushHeadIdx(head, len, N, err) \
	do { \
		if (len == N) { \
			csnip_err_Raise(csnip_err_NOMEM, err); \
			break; \
		} \
		csnip_ringbuf__SubWrapSet(N, 1, head); \
		++len; \
	} while (0)
/** @endcond */

/** Remove an element from the head. */
#define csnip_ringbuf_PopHeadIdx(head, len, N, err) \
	csnip_ringbuf__PopHeadIdx((head), (len), (N), (err))

/** @cond */
#define csnip_ringbuf__PopHeadIdx(head, len, N, err) \
	do { \
		if (len == 0) { \
			csnip_err_Raise(csnip_err_UNDERFLOW, err); \
			break; \
		} \
		csnip_ringbuf__AddWrapSet(N, 1, head); \
		--len; \
	} while (0)
/** @endcond */

/** Add an element to the tail.
 *
 *  Similar to csnip_ringbuf_PushHeadIdx() but adding at the tail.
 */
#define csnip_ringbuf_PushTailIdx(head, len, N, err) \
	csnip_ringbuf__PushTailIdx((head), (len), (N), (err))

/** @cond */
#define csnip_ringbuf__PushTailIdx(head, len, N, err) \
	do { \
		if (len == N) { \
			csnip_err_Raise(csnip_err_NOMEM, err); \
			break; \
		} \
		++len; \
	} while (0)
/** @endcond */

/** Remove an element from the tail. */
#define csnip_ringbuf_PopTailIdx(head, len, N, err) \
	csnip_ringbuf__PopTailIdx((head), (len), (N), (err))

/** @cond */
#define	csnip_ringbuf__PopTailIdx(head, len, N, err) \
	do { \
		if (len == 0) { \
			csnip_err_Raise(csnip_err_UNDERFLOW, err); \
			break; \
		} \
		--len; \
	} while (0)
/** @endcond */

/** Check whether a given index is valid.
 *
 *  Checks whether a given index points to within the ring buffer as
 *  currently occupied.  A range error is raised if this is not the
 *  case; otherwise nothing happens.
 */
#define csnip_ringbuf_CheckIdx(head, len, N, idx, err) \
	csnip_ringbuf__CheckIdx((head), (len), (N), (idx), (err))

/** @cond */
#define csnip_ringbuf__CheckIdx(head, len, N, idx, err) \
	do { \
		if ((idx) >= (head)) { \
			if ((idx) - (head) >= (len)) { \
				csnip_err_Raise(csnip_err_RANGE, err); \
			} \
		} else { \
			if ((idx) - (head) + (N)  >= (len)) { \
				csnip_err_Raise(csnip_err_RANGE, err); \
			} \
		} \
	} while (0)
/** @endcond */

/** Compute the sum @a idx + @a amount with cyclic wrap.
 *
 *  Expression macro.  Increases @a idx by @a amount, wrapping around
 *  at the array boundary.  Thus, this is similar to computing
 *  \code
 *	(idx + amount) % N
 *  \endcode
 *  with the exceptions that possible overflow is handled correctly, and
 *  that the amount can be negative.  (However, the allowable range for
 *  @a base and @a amount is restricted as described below.)
 *
 *  Increasing the index by a positive amount corresponds to moving
 *  toward the tail.
 *
 *  @param[in]	N
 *		the modulus, i.e., the size of the backing array.
 *
 *  @param[in]	amount
 *		the number of entries to advance @a base. Needs
 *		to be in the range -@a N < @a amount < @a N.
 *
 *  @param[in]	idx
 *		the index to advance from.  Needs to be in the range
 *		0 <= @a idx < @a N.
 */
#define csnip_ringbuf_AddWrap(N, amount, idx) \
	((amount) >= 0 ? csnip_ringbuf__AddWrap((N), (amount), (idx)) \
		: csnip_ringbuf__SubWrap((N), -(amount), (idx)))

/** Compute @a idx += @a amount.
 *
 *  Expression Macro.  Similar to csnip_ringbuf_AddWrap(), except the
 *  result is assigned to @a idx.
 */
#define csnip_ringbuf_AddWrapSet(N, amount, idx) \
	((amount) >= 0 ? csnip_ringbuf__AddWrapSet((N), (amount), (idx)) \
		: csnip_ringbuf__SubWrapSet((N), -(amount), (idx)))

/** Compute @a idx - @a amount with cyclic wrap.
 *
 *  Expression macro.  Subtracts @a amount from @a amount, wrapping around
 *  at the array boundary.  Thus, this is similar to computing
 *  \code
 *	(idx - amount + N) % N
 *  \endcode
 *  with the exceptions that possible overflow is handled correctly.
 *  (However, the allowable range for @a base and @a amount is
 *  restricted as described below.)
 *
 *  Decreasing the index amount by a positive amount results in a move
 *  toward the head.
 *
 *  @param[in]	N
 *		size of the ring buffer
 *
 *  @param[in]	amount
 *		the number of entries to subtract from @a idx.  Needs to
 *		be in the range -@a N < @a amount < @a N.
 *
 *  @param[in]	idx
 *		the index to advance from.  Needs to be in the range
 *		0 <= @a idx < @a N.
 */
#define csnip_ringbuf_SubWrap(N, amount, idx) \
	((amount) >= 0 ? csnip_ringbuf__SubWrap((N), (amount), (idx)) \
		: csnip_ringbuf__AddWrap((N), -(amount), (idx)))

/** Compute @a idx -= @a amount with cyclic wrap.
 *
 *  Expression Macro.  Similar to csnip_ringbuf_SubWtap(), except that
 *  the result is assigned to @a idx.
 */
#define csnip_ringbuf_SubWrapSet(N, amount, idx) \
	((amount) >= 0 ? csnip_ringbuf__SubWrapSet((N), (amount), (idx)) \
		: csnip_ringbuf__AddWrapSet((N), -(amount), (idx)sebase))

/** @cond */
#define csnip_ringbuf__AddWrap(N, amount, idx) \
	(idx < N - amount ? idx + amount : amount - (N - idx))

#define csnip_ringbuf__AddWrapSet(N, amount, idx) \
	(idx < N - amount ? (idx += amount) \
				: (idx = amount - (N - idx)))

#define csnip_ringbuf__SubWrap(N, amount, idx) \
	(idx >= amount ? idx - amount : idx - amount + N)

#define csnip_ringbuf__SubWrapSet(N, amount, idx) \
	(idx >= amount ? (idx -= amount) : (idx = idx - amount + N))
/** @endcond */

/** Add an element to the head.
 *
 *  This is a value-version of csnip_ringbuf_PushHeadIdx which also
 *  performs the assignment of @a val to @a the new head location of the
 *  backing array @a arr.
 */
#define csnip_ringbuf_PushHead(head, len, N, arr, val, err) \
	do { \
		int csnip__err2 = 0; \
		csnip_ringbuf_PushHeadIdx(head, len, N, csnip__err2); \
		if (csnip__err2 != 0) { \
			csnip_err_Raise(csnip__err2, err); \
			break; \
		} \
		(arr)[head] = (val); \
	} while (0)

/** Remove an element from the head.
 *
 *  Like csnip_ringbuf_PopHeadIdx, except that the removed head element
 *  is assigned to @a ret .
 */
#define csnip_ringbuf_PopHead(head, len, N, arr, ret, err) \
	do { \
		(ret) = (arr)[head]; \
		csnip_ringbuf_PopHeadIdx(head, len, N, err); \
	} while (0)

/** Add an element to the tail.
 *
 *  Like csnip_ringbuf_PushHead(), but for the tail.
 */
#define csnip_ringbuf_PushTail(head, len, N, arr, val, err) \
	do { \
		int csnip__err2 = 0; \
		size_t csnip__i; \
		csnip_ringbuf_PushTailIdx(head, len, N, csnip__err2); \
		if (csnip__err2 != 0) { \
			csnip_err_Raise(csnip__err2, err); \
			break; \
		} \
		csnip_ringbuf_GetTailIdx(head, len, N, csnip__i, csnip__err2); \
		if (csnip__err2 != 0) { \
			csnip_err_Raise(csnip__err2, err); \
			break; \
		} \
		(arr)[csnip__i] = (val); \
	} while (0)

/** Remove an element from the tail.
 *
 *  Like csnip_ringbuf_PopHead(), except that it's acting on the tail.
 */
#define csnip_ringbuf_PopTail(head, len, N, arr, ret, err) \
	do { \
		int csnip__err2 = 0; \
		size_t csnip__i; \
		csnip_ringbuf_GetTailIdx(head, len, N, csnip__i, csnip__err2); \
		if (csnip__err2 != 0) { \
			csnip_err_Raise(csnip__err2, err); \
			break; \
		} \
		(ret) = (arr)[csnip__i]; \
		csnip_ringbuf_PopTailIdx(head, len, N, err); \
	} while (0)

/**	Generator macro to declare index functions.
 *
 *	@param	scope
 *		Scope in which the functions are declared.
 *
 *	@param	prefix
 *		Prefix to the name of the functions declared.
 *
 *	@param	idx_type
 *		Type used for indexing.
 *
 *	@param	gen_args
 *		Generic argument list.
 *
 *	For details, @sa CSNIP_RINGBUF_DEF_IDX_FUNCS().
 */
#define CSNIP_RINGBUF_DECL_IDX_FUNCS(scope, prefix, idx_type, gen_args) \
	scope void prefix##init(csnip_pp_list_##gen_args); \
	scope idx_type prefix##get_head_idx(csnip_pp_list_##gen_args); \
	scope idx_type prefix##get_tail_idx(csnip_pp_list_##gen_args); \
	scope void prefix##push_head_idx(csnip_pp_list_##gen_args); \
	scope void prefix##pop_head_idx(csnip_pp_list_##gen_args); \
	scope void prefix##push_tail_idx(csnip_pp_list_##gen_args); \
	scope void prefix##pop_tail_idx(csnip_pp_list_##gen_args); \
	scope void prefix##check_idx(csnip_pp_prepend_##gen_args idx_type); \
	scope idx_type prefix##add_wrap(csnip_pp_prepend_##gen_args \
				idx_type, idx_type); \
	scope idx_type prefix##sub_wrap(csnip_pp_prepend_##gen_args \
				idx_type, idx_type); \

/**	Declare value functions.
 *
 *	Generator macro to declare value functions.
 *
 *	@param	scope
 *		function scope.
 *
 *	@param	prefix
 *		function name prefix.
 *
 *	@param	val_type
 *		ringbuffer value type.
 *
 *	@param	gen_args
 *		generic arguments to pass to the ringbuffer functions.
 *
 *	For details, @sa CSNIP_RINGBUF_DEF_VAL_FUNCS().
 */
#define CSNIP_RINGBUF_DECL_VAL_FUNCS(scope, prefix, val_type, gen_args) \
	scope void prefix##push_head(csnip_pp_prepend_##gen_args \
					val_type val); \
	scope val_type prefix##pop_head(csnip_pp_list_##gen_args); \
	scope void prefix##push_tail(csnip_pp_prepend_##gen_args \
					val_type val); \
	scope val_type prefix##pop_tail(csnip_pp_list_##gen_args);

/**	Define Ringbuffer index functions.
 *
 *	Generator macro to define the index computation functions for a
 *	ring buffer.
 *
 *	@param	scope
 *		Function scope.
 *
 *	@param	prefix
 *		Function name prefixes for the defined functions.
 *
 *	@param	idx_type
 *		Type used for indexing; common choices are int or
 *		size_t.
 *
 *	@param	gen_args
 *		General argument list (function prototype).
 *
 *	@param	head
 *		The head variable of the ring buffer; can be a function
 *		of the gen_args arguments.
 *
 *	@param	len
 *		The number of members currently contained in the
 *		ring buffer; can be a function of the gen_args arguments
 *
 *	@param	N
 *		The size of the ring buffer, can be a function of the
 *		gen_args arguments.
 *
 *	@param	err
 *		The error return value; can be a function of the
 *		gen_args arguments.
 *
 *	The following functions are defined:
 *
 *	* void init(gen_args):  Zero-initialize the indices.
 *	  @sa csnip_ringbuf_Init()
 *
 *	* size_t get_head_idx(gen_args):  Get the head index.
 *	  @sa csnip_ringbuf_GetHeadIdx()
 *
 *	* size_t get_tail_idx(gen_args):  Get the tail index.
 *	  @sa csnip_ringbuf_GetTailIdx()
 *
 *	* void push_head_idx(gen_args):  Push to the head.
 *	  @sa csnip_ringbuf_PushHeadIdx()
 *
 *	* void pop_head_idx(gen_args):  Pop from the head.
 *	  @sa csnip_ringbuf_PopHeadIdx()
 *
 *	* void push_tail_idx(gen_args):  Push to the tail.
 *	  @sa csnip_ringbuf_PushTailIdx()
 *
 *	* void pop_tail_idx(gen_args):  Pop from the tail; wraps.
 *	  @sa csnip_ringbuf_PopTailIdx()
 *
 *	* void check_idx(gen_args, idx_type i):  Check if the index i
 *	  points to within a currently occupied slot of the ring buffer.
 *	  @sa csnip_ringbuf_CheckIdx()
 *
 *	* idx_type add_wrap(gen_args, idx_type off, idx_type i): Add
 *	  off to i modulo N.
 *	  @sa csnip_ringbuf_AddWrap()
 *
 *	* idx_type sub_wrap(gen_args, idx_type off, idx_type i): Add
 *	  off from i modulo N.
 *	  @sa csnip_ringbuf_SubWrap()
 */
#define CSNIP_RINGBUF_DEF_IDX_FUNCS(scope, prefix, idx_type, gen_args, \
	head, len, N, err) \
	scope void prefix##init(csnip_pp_list_##gen_args) { \
		csnip_ringbuf_Init(head, len, N); \
	} \
	scope idx_type prefix##get_head_idx(csnip_pp_list_##gen_args) { \
		idx_type csnip__ret; \
		csnip_ringbuf_GetHeadIdx(head, len, N, csnip__ret, err); \
		return csnip__ret; \
	} \
	scope idx_type prefix##get_tail_idx(csnip_pp_list_##gen_args) { \
		idx_type csnip__ret; \
		csnip_ringbuf_GetTailIdx(head, len, N, csnip__ret, err); \
		return csnip__ret; \
	} \
	scope void prefix##push_head_idx(csnip_pp_list_##gen_args) { \
		csnip_ringbuf_PushHeadIdx(head, len, N, err); \
	} \
	scope void prefix##pop_head_idx(csnip_pp_list_##gen_args) { \
		csnip_ringbuf_PopHeadIdx(head, len, N, err); \
	} \
	scope void prefix##push_tail_idx(csnip_pp_list_##gen_args) { \
		csnip_ringbuf_PushTailIdx(head, len, N, err); \
	} \
	scope void prefix##pop_tail_idx(csnip_pp_list_##gen_args) { \
		csnip_ringbuf_PopTailIdx(head, len, N, err); \
	} \
	scope void prefix##check_idx(csnip_pp_prepend_##gen_args \
					idx_type csnip__i) \
	{ \
		csnip_ringbuf_CheckIdx(head, len, N, csnip__i, err); \
	} \
	scope idx_type prefix##add_wrap(csnip_pp_prepend_##gen_args \
			idx_type csnip__amount, idx_type csnip__base) \
	{ \
		return csnip_ringbuf_AddWrap(N, csnip__amount, csnip__base); \
	} \
	scope idx_type prefix##sub_wrap(csnip_pp_prepend_##gen_args \
			idx_type csnip__amount, idx_type csnip__base) \
	{ \
		return csnip_ringbuf_SubWrap(N, csnip__amount, csnip__base); \
	}

/**	Define Ringbuffer value functions.
 *
 *	Generator macro to define ringbuffer value functions.  The
 *	functions defined are:
 *
 *	* `void push_head(gen_args, val);`
 *
 *	* `val_type pop_head(gen_args, val);`
 *
 *	* `void push_tail(gen_args, val);`
 *
 *	* `val_type pop_tail(gen_args);`
 */
#define CSNIP_RINGBUF_DEF_VAL_FUNCS(scope, prefix, val_type, \
	gen_args, head, len, N, arr, err) \
	scope void prefix##push_head(csnip_pp_prepend_##gen_args \
					val_type val) { \
		csnip_ringbuf_PushHead(head, len, N, arr, val, err); \
	} \
	scope val_type prefix##pop_head(csnip_pp_list_##gen_args) { \
		val_type csnip__ret; \
		csnip_ringbuf_PopHead(head, len, N, arr, csnip__ret, err); \
		return csnip__ret; \
	} \
	scope void prefix##push_tail(csnip_pp_prepend_##gen_args \
					val_type val) { \
		csnip_ringbuf_PushTail(head, len, N, arr, val, err); \
	} \
	scope val_type prefix##pop_tail(csnip_pp_list_##gen_args) { \
		val_type csnip__ret; \
		csnip_ringbuf_PopTail(head, len, N, arr, csnip__ret, err); \
		return csnip__ret; \
	}

/** @} */

#endif /* CSNIP_RINGBUF_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_RINGBUF_HAVE_SHORT_NAMES)
#define ringbuf_Init		csnip_ringbuf_Init
#define ringbuf_GetHeadIdx	csnip_ringbuf_GetHeadIdx
#define ringbuf_GetTailIdx	csnip_ringbuf_GetTailIdx
#define ringbuf_PushHeadIdx	csnip_ringbuf_PushHeadIdx
#define ringbuf_PopHeadIdx	csnip_ringbuf_PopHeadIdx
#define ringbuf_PushTailIdx	csnip_ringbuf_PushTailIdx
#define ringbuf_PopTailIdx	csnip_ringbuf_PopTailIdx
#define ringbuf_CheckIdx	csnip_ringbuf_CheckIdx
#define ringbuf_AddWrap		csnip_ringbuf_AddWrap
#define ringbuf_AddWrapSet	csnip_ringbuf_AddWrapSet
#define ringbuf_SubWrap		csnip_ringbuf_SubWrap
#define ringbuf_SubWrapSet	csnip_ringbuf_SubWrapSet
#define ringbuf_PushHead	csnip_ringbuf_PushHead
#define ringbuf_PopHead		csnip_ringbuf_PopHead
#define ringbuf_PushTail	csnip_ringbuf_PushTail
#define ringbuf_PopTail		csnip_ringbuf_PopTail
#define CSNIP_RINGBUF_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_RINGBUF_HAVE_SHORT_NAMES */
