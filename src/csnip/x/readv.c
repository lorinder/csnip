#include <stdlib.h>
#include <errno.h>

#include <csnip/csnip_conf.h>

#include <csnip/util.h>
#include <csnip/x.h>
#include <csnip/x_unistd.h>

csnip_x_ssize_t csnip_x_readv_imp(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt)
{
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
		size_t ncp = csnip_Min(iov[i].iov_len, (size_t)(end - q));
		memcpy(iov[i].iov_base, q, ncp);
		q += ncp;
	}

	/* Free temp memory */
	free(p);
	return r;
}
