#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/rng_mt.h>

int main()
{
	rng_mt_state S;
	rng_mt_seed(&S, 1, (uint32_t[]){ 1234567 });

	/* Print the first few values */
	for (int i = 0; i < 10; ++i) {
		printf("%lu\n", (unsigned long)rng_mt_getnum(&S));
	}

	/* Advance and then print a later value */
	puts("...");
	for(int i = 0; i < 1000000000; ++i)
		rng_mt_getnum(&S);
	unsigned long v = rng_mt_getnum(&S);
	printf("%lu\n", v);

	/* Check the obtained value */
	int fail = 0;
	if (v != 3209008604u)
		fail = 1;

	return fail;
}
