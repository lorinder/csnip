/*
 *  tests for the runif_Geti() macro and related functionality.
 */

#include <stdio.h>
#include <math.h>

#define CSNIP_SHORT_NAMES
#include <csnip/runif.h>
#include <csnip/rng.h>
#include <csnip/rng_mt.h>
#include <csnip/mem.h>

/* smallval_rng
 *
 * creates random numbers in a small range of values.
 * this testing rng uses MT as the backend.  It is slightly biased
 * because of the extreme simplicity of its implementation.
 * This is intentional so we don't base it on API calls (such as
 * runif_Geti()) that we wish to test.
 */

typedef struct {
	unsigned long int modulus;
	rng_mt_state mt_state;
} rng_sv_state;

static void sv_seed(const rng* R,
		int nseed,
		const unsigned long* seed)
{
	rng_sv_state* S = R->state;
	rng_mt_seed(&S->mt_state,
		nseed * sizeof(unsigned long) / sizeof(uint32_t),
		(const uint32_t*)seed);
}

static unsigned long int sv_getnum(const rng* R)
{
	rng_sv_state* S = R->state;
	const unsigned long int
	  num = rng_mt_getnum(&S->mt_state);
	return num % S->modulus;
}

rng make_sv_rng(int maxval, rng_sv_state* S)
{
	const uint32_t v = 1;
	rng_mt_seed(&S->mt_state, 1, &v);
	S->modulus = maxval + 1;

	return (rng){
		.minval = 0,
		.maxval = maxval,
		.state = S,
		.seed = sv_seed,
		.getnum = sv_getnum
	};

}

/***/

void test_rand01(const rng* rng)
{
	puts("Sampling random 0, 1 values from smallval_rng");
	const int N = 1000000;

	int n1 = 0;
	for (int i = 0; i < N; ++i) {
		int v = runif_Geti(rng, 1);
		if (v != 0 && v != 1) {
			fprintf(stderr,
			  "Error: Invalid value %d "
			  "found.\n", v);
			fprintf(stderr, "       "
			  "Expected 0 or 1.\n");
			exit(1);
		}
		n1 += v;
	}

	printf("Got %d ones out of %d samples.\n",
		n1, N);
	float E = N / 2.f;
	float var = N / 4.f;
	float stddev = sqrtf(var);
	float nd = (n1 - E) / stddev;
	printf("Distance in standard deviations "
	       "is %g\n", nd);
	puts("");
}

void test_rand100(const rng* R)
{
	puts("Sampling some values in the range [0, 99].");
	for(int i = 0; i < 10; ++i) {
		printf(" %d,", runif_Geti(R, 99));
	}
	putchar('\n');

	const int N = 1000000;
#define Ncat 100
	int nhit[Ncat] = {};
	for (int i = 0; i < N; ++i) {
		int v = runif_Geti(R, Ncat - 1);
		if (v < 0 || v >= Ncat) {
			fprintf(stderr, "Error: runif_Geti() returned a "
					"value out of range.\n");
			fprintf(stderr, "       Got %d when expecting the range [0, 99].\n", v);
			exit(1);
		}
		nhit[v]++;
	}

	/* Display result table */
	printf("Occurrence count for N = %d samples.\n", N);
	for (int i = 0; i < Ncat; ++i) {
		printf("[%2d] %5d ", i, nhit[i]);
		if (i % 7 == 6)
			putchar('\n');
	}
	putchar('\n');

	/* Compute the test statistic */
	const double p0 = 1. / Ncat;
	const double mean0 = N * p0;
	const double var0 = N * p0 * (1 - p0);
	const double sd0 = sqrt(var0);
	//printf("H0: mean = %g, sd = %g\n", mean0, sd0);

	double s = 0;
	for (int i = 0; i < Ncat; ++i) {
		const double u = (nhit[i] - mean0)/sd0;
		s += u * u;
	}
	printf("-> test statistic Chi2 (degf = %d) : %g\n", Ncat - 1, s);
#undef Ncat
}

int main()
{
	rng_sv_state state;
	const rng sv = make_sv_rng(2, &state);

	/* seed it */
	const unsigned long seed = 1;
	rng_seed(&sv, 1, &seed);

	/* test 0 */
	puts("Sample output from smallval_rng:");
	for (int i = 0; i < 10; ++i) {
		printf(" %lu,", rng_getnum(&sv));
	}
	putchar('\n');

	/* random 0, 1 values */
	test_rand01(&sv);

	/* random integer in 0, ..., 99 */
	test_rand100(&sv);

	return 0;
}
