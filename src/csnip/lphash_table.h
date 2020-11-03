#ifndef CSNIP_LPHASH_TABLE
#define CSNIP_LPHASH_TABLE

/**	@file lphash_table.h
 *	@addtogroup hash_tables		Hash tables
 *	@{
 *	@defgroup lphash_table		Linear Probing Hash Table
 *	@{
 *
 *	Hash tables based on linear probing.
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <csnip/mem.h>
#include <csnip/preproc.h>
#include <csnip/lphash.h>

/**	Defines a hash table type.
 *
 *	This defines a struct tbltype type, suitable for use as a hash
 *	table with entries of the given type.
 *
 *	@param	struct_tbltype
 *		Name of the struct to be defined.
 *
 *	@param	entrytype
 *		Type of the hash table entries.
 */
#define CSNIP_LPHASH_TABLE_DEF_TYPE(struct_tbltype, \
				entrytype) \
	struct struct_tbltype { \
		size_t cap;		/* Capacity */ \
		size_t size;		/* Number of used entries */ \
		entrytype* entry;	/* The table entries */ \
		unsigned char* occ;	/* Occupancy indicators */ \
	};

#define CSNIP_LPHASH_TABLE_DECL_FUNCS(scope, \
				prefix, \
				keytype, \
				entrytype, \
				tbltype) \
	/* Creation & Deletion */ \
	scope tbltype* prefix##make(int* err); \
	scope void prefix##free(tbltype* tbl); \
	\
	/* Element manipulation */ \
	scope int prefix##insert( \
			tbltype* tbl, \
			int* err, \
			entrytype E); \
	scope int prefix##insert_or_assign( \
			tbltype* tbl, \
			int* err, \
			entrytype E, \
			entrytype* ret_old); \
	scope entrytype* prefix##find_or_insert( \
			tbltype* tbl, \
			int* err, \
			entrytype E); \
	scope _Bool prefix##remove( \
			tbltype* tbl, \
			int* err, \
			keytype key); \
	scope entrytype* prefix##find( \
			const tbltype* tbl, \
			keytype key); \
	\
	/* Size and capacity */ \
	scope size_t prefix##size(const tbltype* tbl); \
	scope size_t prefix##capacity(const tbltype* tbl); \
	\
	/* Slot functions */ \
	scope size_t prefix##findslot( \
			const tbltype* tbl, \
			keytype key); \
	scope _Bool prefix##isslotoccupied( \
			const tbltype* tbl, \
			size_t i); \
	scope entrytype* prefix##getslotentryaddress( \
			const tbltype* tbl, \
			size_t i); \
	scope size_t prefix##getslotfromentryaddress( \
			const tbltype* tbl, \
			entrytype const * entry); \
	scope size_t prefix##removeatslot( \
			tbltype* tbl, \
			int* err, \
			size_t i); \
	scope size_t prefix##firstoccupiedslot( \
			const tbltype* tbl); \
	scope size_t prefix##nextoccupiedslot( \
			const tbltype* tbl, \
			size_t i);

#define CSNIP_LPHASH_TABLE_DEF_FUNCS(scope, \
				prefix, \
				keytype, \
				entrytype, \
				tbltype, \
				k1, k2,		/* key dummy vars */ \
				e,		/* entry dummy var */ \
				hash,		/* evaluate to hash(k1) */ \
				is_match,	/* Check whether k1 and k2 match */ \
				get_key)	/* evaluate to the key of e */ \
	\
	/* Declare functions in case they weren't yet. */ \
	CSNIP_LPHASH_TABLE_DECL_FUNCS(scope, prefix, keytype, entrytype, \
	  tbltype) \
	\
	/* Private methods */ \
	static size_t prefix##_internal_findloc( \
				const tbltype* T, \
				keytype key, \
				int* state_) \
	{ \
		size_t ret_; \
		entrytype e; \
		keytype k2; \
		csnip_lphash_Find(T->cap, keytype, k1, u, \
				hash, \
				!T->occ[u], \
				(e = T->entry[u], k2 = (get_key), (is_match)), \
				(e = T->entry[u], (get_key)), \
				key, \
				ret_, \
				*state_); \
		return ret_; \
	} \
	\
	static void prefix##_internal_deleteloc(tbltype* T, \
						size_t loc) \
	{ \
		entrytype e; \
		csnip_lphash_Delete(T->cap, keytype, k1, u, v, \
				hash, \
				!T->occ[u], \
				(e = T->entry[u], (get_key)), \
				(T->entry[v] = T->entry[u], T->occ[v] = T->occ[u]), \
				T->occ[u] = 0,\
				loc); \
	} \
	\
	static _Bool prefix##_internal_grow(tbltype* T, \
						int* err, \
						size_t min_size) \
	{ \
		if (min_size * 3 <= T->cap * 2) { \
			/* No need to grow */ \
			return 0; \
		} \
		\
		/* Compute new capacity */ \
		size_t newcap = (T->cap ? T->cap : 8); \
		while (min_size * 3 > newcap * 2) { \
			newcap *= 2; \
			/* XXX: Check overflow in the above */ \
		} \
		\
		/* Allocate new hashing table */ \
		entrytype* newarr; \
		unsigned char* newocc; \
		csnip_mem_Alloc(newcap, newarr, *err); \
		if (err && *err) return 0; \
		csnip_mem_Alloc(newcap, newocc, *err); \
		if (err && *err) return 0; \
		tbltype N = { \
			.cap = newcap, \
			.size = T->size, \
			.entry = newarr, \
			.occ = newocc \
		}; \
		for (size_t i = 0; i < newcap; ++i) { \
			newocc[i] = 0; \
		} \
		\
		/* Copy from old to new */ \
		for (size_t i = 0; i < T->cap; ++i) { \
			if (T->occ[i]) { \
				size_t l; \
				int r; \
				entrytype e = T->entry[i]; \
				l = prefix##_internal_findloc(&N, \
						(get_key), &r); \
				assert(r == 1); \
				newarr[l] = T->entry[i]; \
				newocc[l] = 1; \
			} \
		} \
		\
		/* Replace old table with new one, and free */ \
		if (T->entry) csnip_mem_Free(T->entry); \
		if (T->occ) csnip_mem_Free(T->occ); \
		*T = N; \
		\
		return 1; \
	} \
	\
	/* Creation / Deletion */ \
	scope tbltype* prefix##make(int* err) \
	{ \
		if (err) *err = 0; \
		\
		tbltype* T; \
		csnip_mem_Alloc(1, T, *err); \
		if (err && *err) \
			return NULL; \
		T->cap = 0; \
		T->size = 0; \
		T->entry = NULL; \
		T->occ = NULL; \
		return T; \
	} \
	\
	scope void prefix##free(tbltype* T) \
	{ \
		if (T->entry)	csnip_mem_Free(T->entry); \
		if (T->occ)	csnip_mem_Free(T->occ); \
		csnip_mem_Free(T); \
	} \
	\
	/* Element manipulation */ \
	\
	/* Inserts an element only if that key is not yet present.
	 * Returns 0 if nothing was inserted,
	 * returns 1 if a new element was inserted.
	 */ \
	scope int prefix##insert(tbltype* T, int* err, entrytype e) \
	{ \
		if (err) *err = 0; \
		\
		/* Grow if necessary */ \
		prefix##_internal_grow(T, err, T->size + 1); \
		if (err && *err) \
			return 0; \
		\
		/* Insert or replace entry */ \
		int r; \
		keytype key = (get_key); \
		size_t loc = prefix##_internal_findloc(T, key, &r); \
		assert(r < 2); \
		if (r == 1) { \
			T->entry[loc] = e; \
			T->occ[loc] = 1; \
			++T->size; \
		} \
		return r; \
	} \
	\
	/* Replaces existing element, or inserts a new one.
	 *
	 * Returns 0 if element was replaced,
	 * and 1 if element was newly inserted.
	 */ \
	scope int prefix##insert_or_assign(tbltype* T, \
				int* err, \
				entrytype e, \
				entrytype* old) \
	{ \
		if (err) *err = 0; \
		\
		/* Grow if necessary */ \
		prefix##_internal_grow(T, err, T->size + 1); \
		if (err && *err) \
			return 0; \
		\
		/* Insert or assign entry */ \
		int r; \
		keytype key = (get_key); \
		size_t loc = prefix##_internal_findloc(T, key, &r); \
		assert(r < 2); \
		if (r == 0) { \
			if (old) *old = T->entry[loc]; \
		} else { \
			++T->size; \
			T->occ[loc] = 1; \
		} \
		T->entry[loc] = e; \
		return r; \
	} \
	\
	scope entrytype* prefix##find_or_insert(tbltype* T, \
					int* err, \
					entrytype entry) \
	{ \
		if (err) *err = 0; \
		\
		int r; \
		entrytype e = entry; \
		size_t loc = prefix##_internal_findloc(T, (get_key), &r); \
		if (r >= 1) { \
			/* Insert */ \
			if (prefix##_internal_grow(T, err, T->size + 1)) { \
				/* Need to search again, since we
				 * rehashed
				 */ \
				loc = prefix##_internal_findloc(T, \
							(get_key), &r); \
				assert(r == 1); \
			} \
			\
			if (err && *err) \
				return NULL; \
			\
			T->entry[loc] = entry; \
			T->occ[loc] = 1; \
			++T->size; \
		} \
		return &T->entry[loc]; \
	} \
	\
	scope _Bool prefix##remove(tbltype* T, int* err, keytype key) \
	{ \
		int r; \
		const size_t loc = prefix##_internal_findloc(T, key, &r); \
		if (r == 0) { \
			prefix##_internal_deleteloc(T, loc); \
			--T->size; \
		}  \
		return r == 0; \
	} \
	\
	scope entrytype* prefix##find(const tbltype* T, keytype key) \
	{ \
		int r; \
		const size_t loc = prefix##_internal_findloc(T, key, &r); \
		if (r == 0) \
			return &T->entry[loc]; \
		return NULL; \
	} \
	\
	/* Size and capacity */ \
	scope size_t prefix##size(const tbltype* T) \
	{ \
		return T->size; \
	} \
	\
	scope size_t prefix##capacity(const tbltype* T) \
	{ \
		return T->cap; \
	} \
	\
	/* Slot functions */ \
	scope size_t prefix##findslot(const tbltype* T, keytype key) \
	{ \
		int r; \
		const size_t loc = prefix##_internal_findloc(T, key, &r); \
		if (r == 0) \
			return loc; \
		return T->cap; \
	} \
	\
	scope _Bool prefix##isslotoccupied(const tbltype* T, size_t i) \
	{ \
		assert(0 <= i && i < T->cap); \
		return T->occ[i]; \
	} \
	\
	scope entrytype* prefix##getslotentryaddress( \
					const tbltype* T, \
					size_t i) \
	{ \
		return &T->entry[i]; \
	} \
	\
	scope size_t prefix##getslotfromentryaddress( \
					const tbltype* T, \
					entrytype const* entry) \
	{ \
		return (size_t)(entry - T->entry); \
	} \
	\
	scope size_t prefix##removeatslot(tbltype* T, int *err, size_t i) \
	{ \
		if (err) *err = 0; \
		\
		if (T->occ[i]) { \
			prefix##_internal_deleteloc(T, i); \
			--T->size; \
			if (T->occ[i]) \
				return i; \
		} \
		return prefix##nextoccupiedslot(T, i); \
	} \
	\
	scope size_t prefix##firstoccupiedslot(const tbltype* T) \
	{ \
		size_t r; \
		for (r = 0; r < T->cap; ++r) \
			if (T->occ[r]) break; \
		return r; \
	} \
	\
	scope size_t prefix##nextoccupiedslot( \
					const tbltype* T, \
					size_t r) \
	{ \
		for (++r; r < T->cap; ++r) \
			if (T->occ[r]) break; \
		return r; \
	}

/** @}
 *  @}
 */

#endif /* CSNIP_LPHASH_TABLE */
