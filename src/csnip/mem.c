#include <csnip/csnip_conf.h>

#include <errno.h>
#include <stdlib.h>

#if defined(CSNIP_CONF__HAVE_MEMALIGN) \
	|| defined(CSNIP_CONF__HAVE_ALIGNED_MALLOC)
#include <malloc.h>
#endif

#include <csnip/err.h>
#include <csnip/mem.h>

static inline size_t compute_alloc_amount(size_t n, size_t size)
{
	if (size != 0 && SIZE_MAX / size < n) {
		/* Overflow */
		return 0;
	}
	if ((size *= n) == 0) {
		/* malloc() with 0 size is not guaranteed to return a
		 * non-NULL pointer, thus we make sure we allocate
		 * always non-zero amounts.
		 *
		 * Also, we use zero to signal an error.
		 */
		size = 1;
	}
	return size;
}

void* csnip_mem_alloc(size_t n, size_t size)
{
	size_t alloc_sz = compute_alloc_amount(n, size);
	if (alloc_sz == 0)
		return NULL;
	return malloc(alloc_sz);
}

/* For aligned allocation, we use posix_memalign() if possible, since
 * that function has the least restrictions, and best error reporting.
 * Failing that, we try aligned_alloc().  In the worst case, we fall
 * back to memalign().
 */
#if defined(CSNIP_CONF__HAVE_POSIX_MEMALIGN) \
	|| defined(CSNIP_CONF__HAVE_ALIGNED_ALLOC) \
	|| defined(CSNIP_CONF__HAVE_MEMALIGN)

void* csnip_mem_aligned_alloc(size_t nAlign, size_t n, size_t size, int* err_ret)
{
	size_t alloc_sz = compute_alloc_amount(n, size);

#if defined(CSNIP_CONF__HAVE_POSIX_MEMALIGN) \
	|| !defined(CSNIP_CONF__HAVE_ALIGNED_ALLOC)
	if (nAlign < sizeof(void*))
		nAlign *= sizeof(void*);

	void* p_ret;
#ifdef CSNIP_CONF__HAVE_POSIX_MEMALIGN
	const int err = posix_memalign(&p_ret, nAlign, alloc_sz);
#else
	int err = 0;
	p_ret = memalign(nAlign, alloc_sz);
	if (p_ret == NULL)
		err = errno;
#endif
	if (err != 0) {
		p_ret = NULL;
		switch (err) {
		case ENOMEM:
			*err_ret = csnip_err_NOMEM;
			break;
		case EINVAL:
			*err_ret = csnip_err_INVAL;
			break;
		default:
			*err_ret = csnip_err_ERRNO;
			errno = err;
		};
	}
	return p_ret;
#else
	/* use aligned_alloc() */
	const size_t rem = alloc_sz % nAlign;
	if (rem != 0) {
		const size_t toadd = nAlign - rem;
		/* Check for overflow */
		if (SIZE_MAX - toadd < alloc_sz) {
			if (err_ret)
				*err_ret = csnip_err_RANGE;
			return NULL;
		}
		alloc_sz += toadd;
	}
	void* p_ret = aligned_alloc(nAlign, alloc_sz);
	if (p_ret == NULL && err_ret != 0) {
		if (errno == ENOMEM) {
			*err_ret = csnip_err_NOMEM;
		} else {
			*err_ret = csnip_err_ERRNO;
		}
	}
	return p_ret;
#endif
}

void csnip_mem_aligned_free(void* mem)
{
	free(mem);
}

#elif defined(CSNIP_CONF__HAVE__ALIGNED_MALLOC)

/* Windows: aligned allocation is provided with the _aligned_malloc()
 * / _aligned_free() pair of functions.
 */

void* csnip_mem_aligned_alloc(size_t nAlign, size_t n, size_t size, int* err_ret)
{
	size_t alloc_sz = compute_alloc_amount(n, size);
	if (alloc_sz == 0) {
		if (err_ret)
			*err_ret = csnip_err_RANGE;
		return NULL;
	}
	
	if (err_ret)
		*err_ret = 0;
	void* p_ret = _aligned_malloc(alloc_sz, nAlign);
	if (p_ret == NULL && err_ret) {
		*err_ret = csnip_err_NOMEM;
	}
	return p_ret;
}

void csnip_mem_aligned_free(void* mem)
{
	_aligned_free(mem);
}

#endif /* CSNIP_CONF__HAVE_ALIGNED_MALLOC */
