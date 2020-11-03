#ifndef CSNIP_LPHASH_H
#define CSNIP_LPHASH_H

#include <csnip/preproc.h>

/**	@file lphash.h
 *      @addtogroup hash_tables		Hash tables
 *      @{
 *	@defgroup lphash		Linear probing hashing primitives
 *	@{
 *
 *	Primitives for linear probing hash tables.
 *
 *	These are low level macros aiding for the implementation of hash
 *	tables.  In most cases, it will be more convenient to use a
 *	higher level implementation of hash tables such as the one from
 *	lphash_table.h.
 */

/* table description:
 *   - N: table capacity
 *   - key type
 *   - dummy variables k (key), u, v (slot indices)
 *   - hashing expression: k -> hash value.
 *   - is_empty expression: u -> 0 or 1
 *   - is_match expression: u, k -> 0 or 1
 *   - get_key expression u -> key value
 *   - copy: copy or move the u-th entry to the v-th entry
 *   - clear: clear the u-th table entry (mark it as empty)
 *
 * MISSING in the table description
 *   - is_replaceable expression: u -> 0 or 1.
 *     There is still some question whether this is worth doing for
 *     linear probing;  it would cover some more advanced special
 *     purpose applications, but also somewhat complicate the find
 *     algorithms. (Use cases:  Suppose we want to automatically
 *     invalidate entries in a hashing table, without the need to
 *     explicitly delete them.  That could be the case for example
 *     for "old" table entries; that could be implemented by some form
 *     of is_replaceable.  This needs more thought, because you still
 *     want to make sure that a fixed fraction of entries remains truly
 *     empty rather than just replaceable.)
 */

/** Find a slot matching the key or an insertion position.
 *
 *  If there is a slot matching the key, then its location is
 *  returned in return_loc, and return_state will be 0.
 *
 *  If there is no such slot, and the table is not full, return_loc
 *  contains the appropriate location where a new entry with the
 *  given key can be inserted.  In this case, return_state will be
 *  1.
 *
 *  Finally, if no slot matching the key is present in the table,
 *  and the table is completely full so that no insertion point is
 *  present either, the return_state will be 2.
 *
 *	Table description:
 *
 *	@param	N
 *		table capacity
 *
 *	@param	keytype
 *		key data type
 *
 *	@param	k, u
 *		dummy variables.
 *
 *	@param	hash
 *		hashing expression.
 *
 *	@param	is_empty
 *		empty slot check expression.
 *
 *	@param	is_match
 *		slot match expression.
 *
 *	@param	get_key
 *		slot key retrieval expression.
 *
 *	Table lookup parameters:
 *
 *	@param[in]	key
 *			key to look up.
 *
 *	@param[out]	return_loc
 *			lvalue to assign the returned location to.
 *
 *	@param[out]	return_state
 *			lvalue to return the state.
 *			Value  | Meaning
 *			-------|---------------------------------------
 *			0      |found, return_loc contains the location
 *			1      |not found, return_loc contains an insertion location
 *			2      |not found and table completely full.
 *
 */
#define csnip_lphash_Find( \
		N, \
		keytype, \
		k, u, \
		hash, \
		is_empty, \
		is_match, \
		get_key, \
		\
		key, \
		return_loc, \
		return_state) \
	do { \
		/* Special case: Empty table.
		 *
		 * Treat this specially since hash % N is undefined
		 * in this case. */ \
		if((N) == 0) { \
			(return_loc) = -1; \
			(return_state) = 2; \
			break; \
		} \
		\
		/* Get the first location */ \
		keytype k = (key); \
		size_t u = (hash) % (N); \
		size_t last = u; \
		\
		/* Continue until a match is found */ \
		(return_state) = 0; \
		while (!(is_empty) && !(is_match)) \
		{ \
			++u; \
			if (u == (N)) \
				u = 0; \
			if (u == last) { \
				/* Not found & table full. */ \
				(return_state) = 2; \
				break; \
			} \
		} \
		\
		if (is_empty) { \
			(return_state) = 1; \
		} \
		(return_loc) = u; \
	} while(0)

/** Find the next match.
 *
 *  Find next entry matching the key or an insertion point for the
 *  key; @a loc_prev points to the current match.
 *
 *	@param	N
 *		table capacity
 *	@param	keytype
 *		key data type
 *	@param	k, u
 *		dummy variables
 *	@param	hash
 *		hashing expression
 *	@param	is_empty
 *		empty slot check expression
 *	@param	is_match
 *		slot match expression
 *	@param	get_key
 *		slot key retrieval expression
 *
 *	@param	key
 *		key to look up
 *	@param	loc_prev
 *		previous match location
 *	@param	ret_loc
 *		target return location
 *	@param	ret_state
 *		target return state
 */
#define csnip_lphash_Findnext( \
		N, \
		keytype, \
		k, u, \
		hash, \
		is_empty, \
		is_match, \
		get_key, \
		\
		key, \
		loc_prev, \
		ret_loc, \
		ret_state) \
	do { \
		keytype k = (key); \
		size_t u = (loc_prev); \
		size_t last = u; \
		do { \
			++u; \
			if (u == (N)) \
				u = 0; \
			if (u == last) { \
				/* FIXME: wrapped; handle error */ \
				break; \
			} \
		} while(!(is_empty) && !(is_match)); \
		\
		(ret_loc) = u; \
	} while(0)

/** Delete an entry in the hash table.
 *
 *	@param	N
 *		table capacity
 *	@param	keytype
 *		key data type
 *	@param	k, u, v
 *		dummy variables
 *	@param	hash
 *		hashing expression
 *	@param	is_empty
 *		empty slot check expression
 *	@param	get_key
 *		slot key retrieval expression
 *	@param	copy
 *		slot copy expression
 *	@param	clear
 *		slot clear expression
 *
 *	@param	loc
 *		the location to delete
 */
#define csnip_lphash_Delete( \
		N, \
		keytype, \
		k, u, v, \
		hash, \
		is_empty, \
		get_key, \
		copy, \
		clear, \
		\
		loc) \
	do { \
		size_t v = (loc); \
		size_t u = v; \
		do { \
			if (++u == (N)) \
				u = 0; \
			\
			if ((is_empty) || u == v) { \
				u = v; \
				clear; \
				break; \
			} \
			\
			keytype k = (get_key); \
			size_t h = (hash) % (N); \
			if (v - h < u - h) { \
				copy; \
				v = u; \
			} \
		} while(1); \
	} while(0)

/**	Find the next occupied slot.
 *
 *	This finds the smallest slot index &ge; @a loc that is empty.  The
 *	value in ret will be set to the next nonempty slot index or to N
 *	if all subsequent slot indices are empty.
 *
 *	Example:  The following loop will iterate over all occupied slot
 *	indices.
 *	~~~~~~~~~~~~~~~~~~~{.c}
 *	int i = -1;
 *	do {
 *		csnip_lphash_Nextentry(N, u, is_empty, i + 1, i);
 *		if (i == N)
 *			break;
 *		// Do something with slot i here...
 *	} while(1);
 *	~~~~~~~~~~~~~~~~~~~
 */
#define csnip_lphash_Nextentry( \
		N, \
		u, \
		is_empty, \
		\
		loc, ret) \
	do { \
		size_t u = (loc); \
		while ((is_empty) && u < (N)) { \
			++u; \
		} \
		(ret) = u; \
	} while (0)

/**	Declare lphash function prototypes.
 *
 *	Declares functions for the above macros.  See
 *	CSNIP_LPHASH_DEF_FUNCS() for further details.
 */
#define CSNIP_LPHASH_DECL_FUNCS(scope, prefix, keytype, argl) \
	scope size_t prefix ## findloc(csnip_pp_prepend_##argl \
				keytype key, int* state); \
	scope size_t prefix ## findnextloc(csnip_pp_prepend_##argl keytype key, size_t loc); \
	scope void prefix ## deleteloc(csnip_pp_prepend_##argl size_t loc); \
	scope size_t prefix ## nextentry(csnip_pp_prepend_##argl size_t loc);

/**	Define lphash functions.
 *
 *	The functions defined will be (without prefixes):
 *	* findloc, a version of csnip_lphash_Find() wrapped in a
 *	  function.
 *	* findnextloc, a version of csnip_lphash_Findnext() wrapped in a
 *	  function.
 *	* deleteloc, a version of csnip_lphash_Delete() wrapped in a
 *	  function.
 *	* nextentry, a version of csnip_lphash_Nextentry() wrapped in a
 *	  function.
 */
#define CSNIP_LPHASH_DEF_FUNCS(scope, prefix, \
				argl, \
				N, \
				keytype, \
				k, u, v, \
				hash, \
				is_empty, \
				is_match, \
				get_key, \
				copy, \
				clear) \
	scope size_t prefix##findloc( \
		csnip_pp_prepend_##argl \
		keytype key, \
		int* state_) \
	{ \
		size_t ret_; \
		csnip_lphash_Find(N, keytype, k, u, hash, is_empty, is_match, \
			get_key, key, ret_, *state_); \
		return ret_; \
	} \
	\
	scope size_t prefix ## findnextloc( \
		csnip_pp_prepend_##argl \
		keytype key, \
		size_t loc, \
		int* state_) \
	{ \
		size_t ret_; \
		csnip_lphash_Findnext(N, keytype, k, u, hash, is_empty, \
			is_match, get_key, key, loc, ret_, *state_); \
		return ret_; \
	} \
	\
	scope void prefix ## deleteloc(csnip_pp_prepend_##argl size_t loc) \
	{ \
		csnip_lphash_Delete(N, keytype, k, u, v, hash, is_empty, \
			get_key, copy, clear, loc); \
	} \
	\
	scope size_t prefix ## nextentry(csnip_pp_prepend_##argl size_t loc) \
	{ \
		csnip_lphash_Nextentry(N, u, is_empty, loc, loc); \
		return loc; \
	}

/** @}
 *  @}
 */

#endif /* CSNIP_LPHASH_H */
