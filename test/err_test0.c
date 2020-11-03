#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define csnip_err_Unhandled(errnumber) \
	do { \
		if (errnumber == csnip_err_NOMEM) { \
			printf("Success!  csnip_err_NOMEM caught.\n"); \
			exit(0); \
		} \
		fprintf(stderr, "Wrong error number!\n"); \
		exit(1); \
	} while(0)

#include <csnip/err.h>

int main(void)
{
	csnip_err_Raise(csnip_err_NOMEM, _);

	/* Should never end up here */
	return 1;
}
