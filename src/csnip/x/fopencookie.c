#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <errno.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>
#include <csnip/x.h>

#if defined(CSNIP_CONF__HAVE_FOPENCOOKIE)
FILE* x_fopencookie(void* restrict cookie,
			const char* restrict mode,
			x_cookie_io_functions_t io_funcs)
{
	return fopencookie(cookie, mode, io_funcs);
}

#elif defined(CSNIP_CONF__HAVE_FUNOPEN)

struct cookie_wrapper {
	void* cookie;
	x_cookie_io_functions_t io_funcs;
};

static int fun_read(void* cw_, char* buf, int sz)
{
	struct cookie_wrapper* cw = (struct cookie_wrapper*)cw_;
	if (cw->io_funcs.read == NULL) {
		/* Signal EOF */
		errno = 0;
		return 0;
	} else {
		return (*cw->io_funcs.read)(cw->cookie, buf, sz);
	}
}

static int fun_write(void* cw_, const char* buf, int sz)
{
	struct cookie_wrapper* cw = (struct cookie_wrapper*)cw_;
	if (cw->io_funcs.write == NULL) {
		/* Discard data. */
		return sz;
	} else {
		int r = (*cw->io_funcs.write)(cw->cookie, buf, sz);
		return (r == 0 ? -1 : r);
	}
}

static off_t fun_seek(void* cw_, off_t offs, int whence)
{
	struct cookie_wrapper* cw = (struct cookie_wrapper*)cw_;
	const int r = (*cw->io_funcs.seek)(cw->cookie, &offs, whence);
	if (r == -1)
		return -1;
	return offs;
}

static int fun_close(void* cw_)
{
	struct cookie_wrapper* cw = (struct cookie_wrapper*)cw_;
	int r = 0;
	if (cw->io_funcs.close != NULL) {
		r = (*cw->io_funcs.close)(cw->cookie);
	}
	mem_Free(cw);
	return r;
}


FILE* x_fopencookie(void* restrict cookie,
			const char* restrict mode,
			x_cookie_io_functions_t io_funcs)
{
	/* Create the cookie wrapper */
	struct cookie_wrapper* wr;
	int err = 0;
	mem_Alloc(1, wr, err);
	if (err != 0)
		return NULL;
	*wr = (struct cookie_wrapper){
		.cookie = cookie,
		.io_funcs = io_funcs,
	};

	(void)mode;
	return funopen(wr,
		fun_read,
		fun_write,
		io_funcs.seek != NULL ? fun_seek : NULL,
		fun_close);

}
#endif
