#include <stdio.h>
#include <time.h>

#define CSNIP_SHORT_NAMES
#include <csnip/clopts.h>
#include <csnip/mem.h>
#include <csnip/podtypes.h>
#include <csnip/coro.h>
#include <csnip/time.h>

/* direct */

llong comp_direct(llong nIter)
{
	llong v0 = 1;
	for (llong i = 0; i < nIter; ++i) {
		v0 = (v0 * 48271) % ((1ll << 31) - 1);
	}
	return v0;
}

/* coro based */

typedef struct {
	int pc;
	llong v0;
	llong i;
} comp_coro_state;

static llong next_coro(comp_coro_state* S)
{
	CORO_START(S->pc);

	S->v0 = 1;
	for (S->i = 0;; ++S->i) {
		S->v0 = (S->v0 * 48271) % ((1ll << 31) - 1);
		coro_Yield(S->pc, 1, S->v0);
	}

	CORO_END(S->pc)
	return -1;
}

llong comp_coro(llong nIter)
{
	llong v0 = 1;
	comp_coro_state S = { .v0 = v0 };
	for (llong i = 0; i < nIter; ++i) {
		v0 = next_coro(&S);
	}
	return v0;
}

int main(int argc, char** argv)
{
	llong nIter = 1000000;
	int nCoro = 1;

	clopts opts = {
		.description = "Microbenchmark for coroutines"
	};
	clopts_add_defaults(&opts);
	clopts_Addvar(&opts, 'N', "niter",
		"Number of iterations", &nIter, _);
	clopts_Addvar(&opts, 'c', "coroutines",
		"Number of coroutines", &nCoro, _);
	if (clopts_process(&opts, argc - 1, argv + 1, NULL) != 0) {
		return 1;
	}

	/* Counter vars */
	struct timespec t0, t1;
	double d;

	/* Direct */
	clock_gettime(CLOCK_MONOTONIC, &t0);
	llong r = comp_direct(nIter);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	time_Convert(time_Sub(t1, t0), d);
	printf("Direct computation (no coroutines): %g s\n", d);
	printf("  Check: value obtained was %lld\n", r);

	/* Refun */
	clock_gettime(CLOCK_MONOTONIC, &t0);
	r = comp_coro(nIter);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	time_Convert(time_Sub(t1, t0), d);
	printf("Refun computation (stackless coroutines): %g s\n", d);
	printf("  Check: value obtained was %lld\n", r);

	return 0;
}
