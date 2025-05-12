#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define CSNIP_SHORT_NAMES
#include <csnip/arr.h>
#include <csnip/mem.h>
#include <csnip/util.h>
#include <csnip/x.h>

/* Macros to set the actual fopencookie implementation */
#if 1
#  define my_fopencookie		csnip_x_fopencookie
#  define my_cookie_io_functions_t	csnip_x_cookie_io_functions_t
#else
#  define my_fopencookie		fopencookie
#  define my_cookie_io_functions_t	cookie_io_functions_t
#endif

/* Define suitable cookie functions */

typedef struct {
	/** Backing array */
	char* a;

	/** Its size */
	size_t n;

	/** Its capacity */
	size_t cap;

	/** Current read/write position */
	size_t file_offs;
} my_cookie_t;

ssize_t my_read_func(void* cookie_, char* buf, size_t size)
{
	my_cookie_t* cookie = (my_cookie_t*)cookie_;

	/* EOF ? */
	if (cookie->file_offs >= cookie->n) {
		return 0;
	}

	size_t s = Min(size, cookie->n - cookie->file_offs);
	memcpy(buf, &cookie->a[cookie->file_offs], s);
	cookie->file_offs += s;
	return s;
}

ssize_t my_write_func(void* cookie_, const char* buf, size_t size)
{
	my_cookie_t* cookie = cookie_;

	/* Resize the array if necessary */
	size_t least_cap = cookie->file_offs + size;
	if (least_cap > cookie->cap)
		arr_Reserve(cookie->a, cookie->n, cookie->cap, least_cap, _);

	/* Fill in the data */
	if (cookie->file_offs > cookie->n)
		memset(&cookie->a[cookie->n], 0, cookie->file_offs - cookie->n);
	memcpy(&cookie->a[cookie->file_offs], buf, size);

	/* Update counts */
	cookie->file_offs += size;
	cookie->n = Max(cookie->n, cookie->file_offs);

	return size;
}

int my_seek_func(void* cookie_, off_t* offset, int whence)
{
	my_cookie_t* cookie = cookie_;

	/* Compute the new offset */
	off_t new_offs;
	switch (whence) {
	case SEEK_SET:
		new_offs = *offset;
		break;
	case SEEK_CUR:
		new_offs = cookie->file_offs + *offset;
		break;
	case SEEK_END:
		new_offs = cookie->n + *offset;
		break;
	default:
		errno = EINVAL;
		return -1;
	};

	/* Check for errors */
	if (new_offs < 0) {
		errno = EINVAL;
		return -1;
	}

	/* Update */
	*offset = new_offs;
	cookie->file_offs = new_offs;

	return 0;
}

int my_close_func(void* cookie_)
{
	(void)cookie_;
	return 0;
}

/* Test case implementation */

bool check_simple_write(void)
{
	my_cookie_io_functions_t io_funcs = {
		.write = my_write_func,
	};
	my_cookie_t a = { NULL };
	FILE* fp = my_fopencookie(&a, "w", io_funcs);

	fprintf(fp, "Hello world\n");
	fprintf(fp, "Here I come\n");

	fclose(fp);

	arr_Push(a.a, a.n, a.cap, '\0', _);
	bool success = true;
	if (strcmp("Hello world\nHere I come\n", a.a) != 0)
		success = false;
	mem_Free(a.a);
	return success;
}

int main(void)
{
	puts("Testing x_fopencookie");

#define RUN_TEST(x) do { \
		puts("Running test " #x); \
		bool run_test_success = x; \
		if (!run_test_success) { \
			puts("-> FAIL"); \
			return 1; \
		} else { \
			puts("-> pass"); \
		} \
	} while (0)

	/* Tests we could perform:
	   [ ] simple writes; check result matches.
	   [ ] simple reads; check result matches.
	   [ ] simple write; followed by seek; followed by simple read,
	       check result matches.
	   [ ] correct behaviour when read is not set.
	   [ ] correct behaviour when write is not set.
	   [ ] correct behaviour when seek is not set.
	   [ ] correct behaviour when close it not set.
	   [ ] writing in mode type "r"
	   [ ] reading in mode type "w"
	 */

	RUN_TEST(check_simple_write());

	return 0;
}
