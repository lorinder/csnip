#ifndef CSNIP_MEMPOOL_H
#define CSNIP_MEMPOOL_H

/**	@file mempool.h
 *	@brief			Memory pools
 *	@defgroup mempool	Memory pools
 *	@{
 *
 *	@brief Memory pools.
 *
 *	Memory pools provide memory areas for allocating fixed size
 *	structs, i.e., they are sort of a specialized malloc for
 *	comparatively small memory blocks of small size.
 *
 *	The advantage they have over malloc() is O(1) operations with
 *	better constants, less memory fragmentation, and better spatial
 *	locality.
 *
 *	If the required capacity is known ahead of time, pool
 *	allocations and deallocations are O(1) time.  When the capacity
 *	is not known, they become amortized O(1) time.
 */

#include <assert.h>
#include <stddef.h>

#include <csnip/arr.h>
#include <csnip/mem.h>

#define CSNIP_MEMPOOL_DEF_TYPE(struct_pooltype, itemtype) \
	struct struct_pooltype { \
		itemtype** slabs; \
		size_t n_slabs; \
		size_t cap_slabs; \
		\
		size_t n_items; /* Number of items allocated */ \
		itemtype* first_free; \
	}; \

#define CSNIP_MEMPOOL_DECL_FUNCS(scope, \
		prefix, \
		itemtype, \
		pooltype) \
	/* Pool allocation and release */ \
	scope pooltype prefix##init_empty(void); \
	scope pooltype prefix##init_with_cap(size_t cap, int* err); \
	scope void prefix##deinit(pooltype* pool); \
	\
	/* Item allocation and release */ \
	scope itemtype* prefix##alloc_item(pooltype* pool, int* err); \
	scope void prefix##free_item(pooltype* pool, itemtype* e);

#define CSNIP_MEMPOOL_DEF_FUNCS(scope, \
		prefix, \
		itemtype, \
		pooltype) \
	\
	_Static_assert(sizeof(itemtype) >= sizeof(itemtype*), \
			"memory pool items too small"); \
	_Static_assert(_Alignof(itemtype) >= _Alignof(itemtype*), \
			"item alignment needs to be that of a pointer."); \
	\
	/* Slab helper */ \
	scope itemtype* prefix##get_slab(size_t n_items, int* err) \
	{ \
		itemtype* sl = NULL; \
		csnip_mem_Alloc(n_items, sl, *err); \
		if (sl == NULL) \
			return NULL; \
		for (size_t i = 0; i < n_items - 1; ++i) \
			*((itemtype**)&sl[i]) = &sl[i + 1]; \
		*((itemtype**)&sl[n_items - 1]) = NULL; \
		return sl; \
	} \
	\
	scope pooltype prefix##init_empty(void) \
	{ \
		return (pooltype) { NULL }; \
	} \
	\
	scope pooltype prefix##init_with_cap(size_t cap, int* err) \
	{ \
		itemtype* sl = prefix##get_slab(cap, err); \
		itemtype** slabs; \
		csnip_mem_Alloc(1, slabs, *err); \
		slabs[0] = sl; \
		return (pooltype) { \
			.slabs = slabs, \
			.n_slabs = 1, \
			.cap_slabs = 1, \
			.n_items = 0, \
			.first_free = sl, \
		}; \
	} \
	\
	scope void prefix##deinit(pooltype* pool) \
	{ \
		for (size_t i = 0; i < pool->n_slabs; ++i) { \
			csnip_mem_Free(pool->slabs[i]); \
		} \
		csnip_mem_Free(pool->slabs); \
		*pool = (pooltype) { NULL }; \
	} \
	\
	/* Item allocation and release */ \
	scope itemtype* prefix##alloc_item(pooltype* pool, int* err) \
	{ \
		/* More memory needed ? */ \
		if (pool->first_free == NULL) { \
			size_t sz = pool->n_items; \
			if (sz < 8) sz = 8; \
			pool->first_free = prefix##get_slab(sz, err); \
		} \
		\
		/* Alloc and return */ \
		itemtype* it = pool->first_free; \
		assert(it != NULL); \
		++pool->n_items; \
		pool->first_free = *((itemtype**)it); \
		return it; \
	} \
	\
	scope void prefix##free_item(pooltype* pool, itemtype* e) \
	{ \
		*((itemtype**)e) = pool->first_free; \
		pool->first_free = e; \
	}

/** @} */

#endif /* CSNIP_MEMPOOL_H */
