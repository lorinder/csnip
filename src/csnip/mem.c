#include <csnip/csnip_conf.h>

#include <errno.h>
#include <stdlib.h>

#if defined(CSNIP_CONF__HAVE_MEMALIGN) \
	|| defined(CSNIP_CONF__HAVE_ALIGNED_MALLOC)
#include <malloc.h>
#endif

#include <csnip/err.h>
#include <csnip/mem.h>

/* For aligned allocation, we use posix_memalign() if possible, since
 * that function has the least restrictions, and best error reporting.
 * Failing that, we try aligned_alloc().  In the worst case, we fall
 * back to memalign().
 */

#if defined(CSNIP_CONF__HAVE_POSIX_MEMALIGN) \
	|| defined(CSNIP_CONF__HAVE_ALIGNED_ALLOC) \
	|| defined(CSNIP_CONF__HAVE_MEMALIGN)

void* csnip_mem_aligned_alloc(size_t nAlign, size_t nSize, int* err_ret)
{
#if defined(CSNIP_CONF__HAVE_POSIX_MEMALIGN) \
	|| !defined(CSNIP_CONF__HAVE_ALIGNED_ALLOC)
	void* p_ret;
#ifdef CSNIP_CONF__HAVE_POSIX_MEMALIGN
	const int err = posix_memalign(&p_ret, nAlign, nSize);
#else
	int err = 0;
	p_ret = memalign(nAlign, nSize);
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
	const size_t rem = nSize % nAlign;
	if (rem != 0) {
		const size_t toadd = nAlign - rem;
		/* Check for overflow */
		if (SIZE_MAX - toadd < nSize) {
			if (err_ret)
				*err_ret = csnip_err_RANGE;
			return NULL;
		}
		nSize += toadd;
	}
	void* p_ret = aligned_alloc(nAlign, nSize);
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

#elif defined(CSNIP_CONF__HAVE_ALIGNED_MALLOC)

/* Windows: aligned allocation is provided with the _aligned_malloc()
 * / _aligned_free() pair of functions.
 */

void* csnip_mem_aligned_alloc(size_t nAlign, size_t nSize, int* err_ret)
{
	if (err_ret)
		*err_ret = 0;
	return _aligned_malloc(nSize, nAlign);
}

void csnip_mem_aligned_free(void* mem)
{
	_aligned_free(mem);
}

#endif /* CSNIP_CONF__HAVE_ALIGNED_MALLOC */
