/*
 *  Tests of the runif_Getd macro.
 */

#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/rng.h>
#include <csnip/rng_mt.h>
#include <csnip/runif.h>

int main()
{
	rng_mt_state state;
	unsigned int seed = 1;
	rng_mt_seed(&state, 1, &seed);
	const rng R = csnip_rng_mt_makerng(&state);
	puts("A set of random numbers");
	for (int i = 0; i < 10; ++i) {
		float d = runif_Getf(&R, 1.f);
		printf("%30a\n", d);
	}
	return 0;
}
