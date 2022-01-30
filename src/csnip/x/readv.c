#include <csnip/csnip_conf.h>

#ifdef CSNIP_CONF__EMULATE_READV
  #include <stdlib.h>
  #include <string.h>

  #include <errno.h>
  #include <unistd.h>
#endif

#ifdef CSNIP_CONF__HAVE_READV
  #include <sys/uio.h>
#endif

#include <csnip/util.h>
#include <csnip/x.h>

#if defined(CSNIP_CONF__HAVE_READV) || defined(CSNIP_CONF__EMULATE_READV)
csnip_x_ssize_t csnip_x_readv(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt)
{
#ifndef CSNIP_CONF__EMULATE_READV
	return readv(fd, iov, iovcnt);
#else
	/* Compute the total block size */
	size_t bsz = 0;
	for (int i = 0; i < iovcnt; ++i)
		bsz += iov[i].iov_len;

	/* Read into a temp buffer */
	char* p = malloc(bsz);
	if (p == NULL) {
		errno = ENOMEM;
		return -1;
	}
	csnip_x_ssize_t r = read(fd, p, bsz);
	if (r < 0) {
		free(p);
		return r;
	}

	/* Scatter received data */
	char* q = p;
	const char* end = p + r;
	for (int i = 0; i < iovcnt && q < end; ++i) {
		size_t ncp = csnip_Min(iov[i].iov_len, end - q);
		memcpy(iov[i].iov_base, q, ncp);
		q += ncp;
	}

	/* Free temp memory */
	free(p);
	return r;
#endif
}
#endif
