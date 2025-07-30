#ifndef CSNIP_MEM_H
#define CSNIP_MEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <csnip/err.h>

/** @file mem.h
 *  @defgroup mem	Memory Managment
 *  @{
 *
 *  Convenient and safe memory allocation macros.
 *
 *  These macros provide an improved interface to the malloc() / free()
 *  libc functions.  They can be intermixed with conventional malloc() /
 *  free() calls, e.g. memory allocated with csnip_mem_Alloc() can be
 *  freed with free(), etc.  Improvements over the conventional malloc
 *  interface include:
 *
 *     1. Automatic element size computation, the frequently used sizeof
 *        construct is builtin, including overflow checks.
 *     2. Works also in C++ without cast.
 *     3. Better error handling in particular for csnip_mem_Realloc().
 */

#ifdef __cplusplus
extern "C" {
#endif

/**	Allocate aligned memory.
 *
 *	The aligned allocator is similar to C11's aligned_alloc, but
 *	does not have the requirement that nSize be a multiple of
 *	nAlign.
 */
void* csnip_mem_aligned_alloc(size_t nAlign, size_t nSize, int* err);

/**	Free aligned memory.
 *
 *	Free memory that was allocated with csnip_mem_aligned_alloc()
 *	or csnip_mem_AlignedAlloc().  On many systems this is equivalent
 *	to free() or csnip's wrappers thereof, but e.g., Windows
 *	requires special function calls to free aligned memory.
 */
void csnip_mem_aligned_free(void* mem);

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus

/* Memory macros, C version.
 */

#include <stdlib.h>

/**	Allocate a member or an array of members.
 *
 *	Allocate memory for the target type of @a ptr; @a nMember
 *	members will be allocated; the allocated pointer is written into
 *	the @a ptr member.
 *
 *	This function uses malloc() under the hood, so it can be freed
 *	with either libc's free() or csnip_mem_Free().
 *
 *	@param	nMember
 *		Number of members in the array to alocate for; use 1 to
 *		allocate a single member.
 *
 *	@param	ptr
 *		The lvalue to assign to.  This must be a pointer type,
 *		and the per-object size allocated is sizeof(*ptr).  If
 *		the allocation fails, a NULL value will be written into
 *		the, in addition to the error being returned or
 *		processed according to the err argument.
 *
 *	@param	err
 *		Error return; accepts the special _ and error_ignore
 *		values, as explained in csnip_err_Raise().
 */
#define csnip_mem_Alloc(nMember, ptr, err) \
	do { \
		if ((nMember) < 0 || \
		    SIZE_MAX / sizeof(*(ptr)) < (nMember)) \
		{ \
			(ptr) = NULL; \
			csnip_err_Raise(csnip_err_RANGE, err); \
			break; \
		} \
		if (((ptr) = malloc(sizeof(*(ptr)) * (nMember))) == NULL) \
			csnip_err_Raise(csnip_err_NOMEM, err); \
	} while(0)

/**	Allocate and zero-initialize an array.
 *
 *	This is similar to csnip_mem_Alloc, except that additionally the
 *	allocated memory is zero-initialized.
 */
#define csnip_mem_Alloc0(nMember, ptr, err) \
	do { \
		if ((nMember) < 0) { \
			(ptr) = NULL; \
			csnip_err_Raise(csnip_err_RANGE, err); \
			break; \
		} \
		if (((ptr) = calloc((nMember), sizeof(*(ptr)))) == NULL) \
			csnip_err_Raise(csnip_err_NOMEM, err); \
	} while(0)

/**	Aligned memory allocation.
 *
 *	This macro is similar to csnip_mem_Alloc, except that the memory
 *	alloc is aligned to nAlign bytes, which must be a power of two.
 *	Unlike C11's aligned_alloc, there is no requirement that the
 *	size of the requested memory block must be a multiple of the
 *	alignment.  This is convenient, since it's frequently a nuisance
 *	to ensure this requirement is satisfied.
 */
#define csnip_mem_AlignedAlloc(nMember, nAlign, ptr, err) \
	csnip_mem__AlignedAlloc((nMember), (nAlign), (ptr), (err))

/** @cond */
#define csnip_mem__AlignedAlloc(nMember, nAlign, ptr, err) \
	do { \
		if (nAlign < 0 || nAlign > SIZE_MAX ||nMember < 0 || \
		    SIZE_MAX / sizeof(*ptr) < nMember) \
		{ \
			ptr = NULL; \
			csnip_err_Raise(csnip_err_RANGE, err); \
			break; \
		} \
		int csnip__err; \
		ptr = csnip_mem_aligned_alloc(nAlign, nMember * sizeof(*ptr), \
			&csnip__err); \
		if (ptr == NULL) \
			csnip_err_Raise(csnip__err, err); \
	} while(0);
/** @endcond */

/**	Reallocate an array.
 *
 *	Csnip's version of realloc().  If the reallocation fails, @a ptr
 *	remains unchanged.
 *
 *	@param[in]	nMember
 *			New array size
 *
 *	@param[in,out]	ptr
 *			Pointer containing the original value upon the
 *			call, and the new value after the call
 *
 *	@param[in,out]	err
 *			error return
 */
#define csnip_mem_Realloc(nMember, ptr, err) \
	csnip_mem__Realloc((nMember), (ptr), (err), csnip__p)

/** @cond */
#define csnip_mem__Realloc(nMember, ptr, err,		p) \
	do { \
		if (nMember < 0 || \
		    SIZE_MAX / sizeof(*ptr) < (size_t)nMember) { \
			csnip_err_Raise(csnip_err_RANGE, err); \
			break; \
		} \
		void* p = realloc(ptr, sizeof(*ptr) * (size_t)nMember); \
		if (p == NULL) { \
			csnip_err_Raise(csnip_err_NOMEM, err); \
			break; \
		} \
		ptr = p; \
	} while(0)
/** @endcond */

/**	Free memory.
 *
 *	Frees the memory pointed-to by @a ptr.  The pointer is cleared
 *	afterward.  The memory must have been allocated with one of the
 *	C allocator functions malloc(), calloc(), realloc() or one of
 *	csnip's mem_Alloc() or similar functions.
 *
 *	However, this macro should not be used to free memory allocated
 *	with the aligned allocators such as csnip_mem_aligned_alloc() or
 *	csnip_mem_AlignedAlloc().  Portable code should use
 *	csnip_mem_AlignedFree() instead.
 *
 *	If the ptr is a null-pointer, no operation is performed.
 *
 */
#define csnip_mem_Free(ptr) \
	do { \
		free(ptr); \
		(ptr) = NULL; \
	} while(0)

/**	Free aligned memory.
 *
 *	Frees the memory pointed-to by @a ptr.  This is for memory that
 *	was alloc allocated with an aligned allocator such as
 *	csnip_mem_aligned_alloc() or csnip_mem_AlignedAlloc().
 *
 *	If ptr is a NULL pointer, no action occurs.
 *
 *	Note:  On POSIX systems this is equivalent to csnip_mem_Free(),
 *	but some systems, such as Windows, require a special deallocator
 *	to be used for aligned allocations;  therefore csnip provides a
 *	deallocator for aligned memory for portability.
 */
#define csnip_mem_AlignedFree(ptr) \
	do { \
		csnip_mem_aligned_free(ptr); \
		(ptr) = NULL; \
	} while (0)

#else
/** @cond
 *
 * Memory macros, C++ version.
 */

#include <cstdlib>

template<typename T>
  inline T* csnip_mem_alloc_cxx(size_t nMemb, T* typeselect, int* err)
{
	(void)typeselect;
	if (SIZE_MAX / sizeof(T) < nMemb) {
		*err = csnip_err_RANGE;
		return NULL;
	}
	return (T*)std::malloc(sizeof(T) * nMemb);
}

template<typename T>
  inline T* csnip_mem_calloc_cxx(size_t nMemb, T* typeselect, int* err)
{
	(void)typeselect;
	if (SIZE_MAX / sizeof(T) < nMemb) {
		*err = csnip_err_RANGE;
		return NULL;
	}
	return (T*)std::calloc(nMemb, sizeof(T));
}

template<typename T>
  inline T* csnip_mem_aligned_alloc_cxx(size_t nMemb, size_t nAlign, T* typeselect, int* err)
{
	(void)typeselect;
	if (SIZE_MAX / sizeof(T) < nMemb) {
		*err = csnip_err_RANGE;
		return NULL;
	}
	return (T*)csnip_mem_aligned_alloc(nAlign, nMemb * sizeof(T), err);
}

template<typename T>
  inline void csnip_mem_realloc_cxx(size_t nMemb, T** orig, int* err)
{
	if (SIZE_MAX / sizeof(T) < nMemb) {
		*err = csnip_err_RANGE;
		return;
	}
	T* new_ptr = (T*)std::realloc(*orig, sizeof(T) * nMemb);
	if (new_ptr) {
		*orig = new_ptr;
	} else {
		*err = csnip_err_NOMEM;
	}
}

#define csnip_mem_Alloc(nMember, ptr, err) \
	do { \
		int csnip__err = 0; \
		(ptr) = NULL; \
		(ptr) = csnip_mem_alloc_cxx((nMember), (ptr), &csnip__err); \
		if ((ptr) == NULL) { \
			csnip_err_Raise(csnip__err, err); \
		} \
	} while(0)

#define csnip_mem_Alloc0(nMember, ptr, err) \
	do { \
		int csnip__err = 0; \
		(ptr) = NULL; \
		(ptr) = csnip_mem_calloc_cxx((nMember), (ptr), &csnip__err); \
		if ((ptr) == NULL) { \
			csnip_err_Raise(csnip__err, err); \
		} \
	} while(0)

#define csnip_mem_AlignedAlloc(nMember, nAlign, ptr, err) \
	do { \
		int csnip__err = 0; \
		(ptr) = NULL; \
		(ptr) = csnip_mem_aligned_alloc_cxx((nMember), (nAlign), \
		  (ptr), &csnip__err); \
		if ((ptr) == NULL) { \
			csnip_err_Raise(csnip__err, err); \
		} \
	} while(0)

#define csnip_mem_Realloc(nMember, ptr, err) \
	do { \
		int csnip__err = 0; \
		csnip_mem_realloc_cxx((nMember), &(ptr), &csnip__err);\
		if (csnip__err) { \
			csnip_err_Raise(csnip__err, err); \
		} \
	} while(0)


#define csnip_mem_Free(ptr) \
	do { \
		std::free(ptr); \
		(ptr) = NULL; \
	} while(0)

#define csnip_mem_AlignedFree(ptr) \
	do { \
		csnip_mem_aligned_free(ptr); \
		(ptr) = NULL; \
	} while (0)

/** @endcond */

#endif

/** @} */

#endif /* CSNIP_MEM_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_MEM_HAVE_SHORT_NAMES)
#define mem_aligned_alloc	csnip_mem_aligned_alloc
#define mem_aligned_free	csnip_mem_aligned_free
#define mem_Alloc		csnip_mem_Alloc
#define mem_Alloc0		csnip_mem_Alloc0
#define mem_AlignedAlloc	csnip_mem_AlignedAlloc
#define mem_Realloc		csnip_mem_Realloc
#define mem_Free		csnip_mem_Free
#define mem_AlignedFree		csnip_mem_AlignedFree
#define CSNIP_MEM_HAVE_SHORT_NAMES
#endif
