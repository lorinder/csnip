#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#define CSNIP_SHORT_NAMES
#include <csnip/arr.h>
#include <csnip/x.h>

#include <csnip/csnip_conf.h>

/* Check if we can use unlocked stdio, and define macros accordingly. */
#if defined(CSNIP_CONF__HAVE_FLOCKFILE) \
    && defined(CSNIP_CONF__HAVE_FUNLOCKFILE) \
    && defined(CSNIP_CONF__HAVE_UNLOCKED_STDIO)
	#define my_flockfile(fp)	flockfile(fp)
	#define my_getc(fp)		getc_unlocked(fp)
	#define my_funlockfile(fp)	funlockfile(fp)
#else
	#define my_flockfile(fp)
	#define my_getc(fp)		getc(fp)
	#define my_funlockfile(fp)
#endif

csnip_x_ssize_t csnip_x_getdelim_imp(char** pline,
			size_t* pcap,
			int delim,
			FILE* fp)
{
	/* Preconditions check */
	if (pline == NULL || pcap == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (*pline == NULL)
		*pcap = 0;

	/* Lock IO */
	my_flockfile(fp);

	/* Read line */
	int c, err = 0;
	size_t sz = 0;
	do {
		/* Read char */
		if ((c = my_getc(fp)) == EOF)
			break;

		/* Append to array */
		arr_Push(*pline, sz, *pcap, c, err);
		if (err)
			goto xit;
	} while (c != delim);
	arr_Push(*pline, sz, *pcap, '\0', err);

	/* Finish, report status */
xit:
	my_funlockfile(fp);
	if (err) {
		errno = ENOMEM;
		return -1;
	} else if (sz == 1) {
		return -1;
	}
	return sz - 1;
}
