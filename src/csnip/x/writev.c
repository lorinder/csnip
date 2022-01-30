#include <csnip/csnip_conf.h>

#ifdef CSNIP_CONF__EMULATE_WRITEV
  #include <stdlib.h>
  #include <string.h>

  #include <errno.h>
  #include <unistd.h>
#endif

#ifdef CSNIP_CONF__HAVE_WRITEV
  #include <sys/uio.h>
#endif

#include <csnip/x.h>

#if defined(CSNIP_CONF__HAVE_WRITEV) || defined(CSNIP_CONF__EMULATE_WRITEV)
csnip_x_ssize_t csnip_x_writev(int fd,
			const struct csnip_x_iovec* iov,
			int iovcnt)
{
#ifndef CSNIP_CONF__EMULATE_WRITEV
	return writev(fd, iov, iovcnt);
#else
	/* Compute the total block size */
	size_t bsz = 0;
	for (int i = 0; i < iovcnt; ++i)
		bsz += iov[i].iov_len;

	/* Copy all the blocks into an temp buffer */
	char* p = malloc(bsz);
	if (p == NULL) {
		errno = ENOMEM;
		return -1;
	}
	char* q = p;
	for (int i = 0; i < iovcnt; ++i) {
		memcpy(q, iov[i].iov_base, iov[i].iov_len);
		q += iov[i].iov_len;
	}

	/* Write */
	csnip_x_ssize_t r = write(fd, p, bsz);

	/* Free, return */
	free(p);
	return r;
#endif
}
#endif
