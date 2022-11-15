/*  Linux: Make sure we get XSI version of strerror_r() */
#define _POSIX_C_SOURCE	200809L
#undef _GNU_SOURCE
#include <string.h>

#include <csnip/csnip_conf.h>

#include <csnip/x.h>

#if defined(CSNIP_CONF__HAVE_STRERROR_R)

int csnip_x_strerror_r(int errnum, char* buf, size_t buflen)
{
	return strerror_r(errnum, buf, buflen);
}
#elif defined(CSNIP_CONF__HAVE_STRERROR_S)

int csnip_x_strerror_r(int errnum, char* buf, size_t buflen)
{
	return strerror_s(buf, buflen, errnum);
}
#endif
