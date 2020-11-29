#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CSNIP_SHORT_NAMES
#include <csnip/coro.h>
#include <csnip/coro_uctx.h>
#include <csnip/coro_pth.h>

char* days[] = {
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday",

	/* Sentinel */
	NULL
};

/* define smoke tests for the different coroutine implementations */
#define DEF_TESTS(V) \
	static void* enumerate_days##V(coro##V* C, void* arg) \
	{ \
		(void)arg; \
		for (int i = 0; days[i]; ++i) { \
			coro##V##_yield(C, days[i]); \
		} \
		return NULL; \
	} \
	\
	static bool test1##V(csnip_coro##V* C) \
	{ \
		puts("Test 1 [" #V "]: Enumeration"); \
		int i = 0; \
		coro##V##_set_func(C, enumerate_days##V, C); \
		do { \
			void* p; \
			coro##V##_next(C, NULL, &p); \
			if (p == NULL) \
				break; \
	\
			/* Check what we got */ \
			if (i >= 7 || strcmp(days[i], p) != 0) \
				return false; \
			puts(p); \
			++i; \
		} while(1); \
	\
		puts("-> passed\n"); \
		return true; \
	} \
	\
	/* Iterate, detect end based on status info */ \
	static bool test2##V(coro##V* C) \
	{ \
		puts("Test 2 [" #V "]: Enumeration, variant 2"); \
		int i = 0; \
		coro##V##_set_func(C, enumerate_days##V, C); \
		do { \
			void* p; \
			if (coro##V##_next(C, NULL, &p) != 0) \
				break; \
	\
			/* Check output */ \
			if (i >= 7 || strcmp(days[i], p) != 0) \
				return false; \
			puts(p); \
			++i; \
		} while(1); \
	\
		puts("-> passed\n"); \
		return true; \
	} \

DEF_TESTS()
#ifdef CSNIP_HAVE_CORO_UCTX
DEF_TESTS(_uctx)
#endif
#ifdef CSNIP_HAVE_CORO_PTH
DEF_TESTS(_pth)
#endif

#undef DEF_TESTS

int main(int argc, char** argv)
{
	bool success = true;
#define RUN_TESTS(V) \
	do { \
		/* Simple enumeration tests */ \
		coro##V* C = coro##V##_new(); \
		if (!test1##V(C)) \
			success = false; \
		if (!test2##V(C)) \
			success = false; \
		coro##V##_free(C); \
	} while (0)

	/* Run tests for the different coro variants */
	RUN_TESTS();
#ifdef CSNIP_HAVE_CORO_UCTX
	RUN_TESTS(_uctx);
#endif
#ifdef CSNIP_HAVE_CORO_PTH
	RUN_TESTS(_pth);
#endif

#undef RUN_TESTS

	/* Clean up */
	return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
