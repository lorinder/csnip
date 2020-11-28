#include <stdio.h>
#include <time.h>

#include <csnip/csnip_conf.h>

#define CSNIP_SHORT_NAMES
#include <csnip/clopts.h>
#include <csnip/coro_pth.h>
#include <csnip/coro_uctx.h>
#include <csnip/mem.h>
#include <csnip/podtypes.h>
#include <csnip/time.h>

llong comp_direct(llong nIter)
{
	llong v0 = 1;
	for (llong i = 0; i < nIter; ++i) {
		v0 = (v0 * 48271) % ((1ll << 31) - 1);
	}
	return v0;
}

/* csnip */

#define DEF_CORO(V) \
	static void* next_##V(csnip_coro_##V* C, void* dummy) \
	{ \
		llong v0 = *(llong*)csnip_coro_##V##_get_value(C); \
		do { \
			v0 = (v0 * 48271) % ((1ll << 31) - 1); \
			v0 = *(llong*)csnip_coro_##V##_yield(C, &v0); \
		} while (1); \
		return NULL; \
	} \
	\
	llong comp_##V(llong nIter, int nCoro) \
	{ \
		csnip_coro_##V** Cs; \
		mem_Alloc(nCoro, Cs, _); \
		for (int i = 0; i < nCoro; ++i) { \
			Cs[i] = csnip_coro_##V##_new(); \
			csnip_coro_##V##_set_func(Cs[i], next_##V, NULL); \
		} \
		\
		llong v0 = 1; \
		for (llong i = 0; i < nIter; ++i) { \
			void* ret; \
			int j = (int)(v0 % nCoro); \
			csnip_coro_##V##_next(Cs[j], &v0, &ret); \
			v0 = *(llong*)ret; \
		} \
		\
		for (int i = 0; i < nCoro; ++i) { \
			csnip_coro_##V##_free(Cs[i]); \
		} \
		mem_Free(Cs); \
		return v0; \
	}

#ifdef CSNIP_HAVE_CORO_UCTX
DEF_CORO(uctx)
#endif
#ifdef CSNIP_HAVE_CORO_PTH
DEF_CORO(pth)
#endif

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

	/* Use uctx implementation */
#ifdef CSNIP_HAVE_CORO_UCTX
	clock_gettime(CLOCK_MONOTONIC, &t0);
	r = comp_uctx(nIter, nCoro);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	time_Convert(time_Sub(t1, t0), d);
	printf("Using ucontext based coroutines: %g s\n", d);
	printf("  Check: value obtained was %lld\n", r);
#endif

	/* Use pth implementation */
#ifdef CSNIP_HAVE_CORO_PTH
	clock_gettime(CLOCK_MONOTONIC, &t0);
	r = comp_pth(nIter, nCoro);
	clock_gettime(CLOCK_MONOTONIC, &t1);
	time_Convert(time_Sub(t1, t0), d);
	printf("Using pth based coroutines: %g s\n", d);
	printf("  Check: value obtained was %lld\n", r);
#endif

	return 0;
}
