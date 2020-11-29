/* This coroutine test covers the special case where the stack needs to
 * be executable.  This is not normally required, but some special
 * features, such as gcc's nested functions require it.
 * The example is a little contrived, so as to force gcc not to inline
 * anything.
 */

#include <stdio.h>

#if defined(__GNUC__) && !defined(__clang__)

#define CSNIP_SHORT_NAMES
#include <csnip/coro.h>
#include <csnip/coro_uctx.h>
#include <csnip/coro_pth.h>

typedef double (*fptr)(double);

double a[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
const int n = sizeof(a) / sizeof(*a);

void do_sth_with_adder(double v, void (*do_sth)(double, fptr))
{
	double adder(double x) {
		return x + v;
	}
	(*do_sth)(v, adder);
}


void do_sth(double x, fptr fun)
{
	printf("With adder (%g):\n", x);
	for (int j = 0; j < n; ++j) {
		printf("  %g", (*fun)(a[j]));
	}
	putchar('\n');
}

#define DEF_F(V) \
	void* f##V(coro##V* C, void* arg) \
	{ \
		(void)C; \
		(void)arg; \
		puts("Running [" #V "]"); \
	\
		for (int i = 0; i < n; ++i) { \
			do_sth_with_adder(a[i], do_sth); \
		} \
	\
		puts("Got through."); \
		return NULL; \
	}

DEF_F()
#ifdef CSNIP_HAVE_CORO_UCTX
DEF_F(_uctx)
#endif
#ifdef CSNIP_HAVE_CORO_PTH
DEF_F(_pth)
#endif

#undef DEF_F

int main()
{
#define TEST(V) do { \
		coro##V* C = coro##V##_new(); \
		coro##V##_set_func(C, f, NULL); \
		coro##V##_next(C, NULL, NULL); \
		coro##V##_free(C); \
	} while(0)

	TEST();
#ifdef CSNIP_HAVE_CORO_UCTX
	TEST(_uctx);
#endif
#ifdef CSNIP_HAVEW_CORO_PTH
	TEST(_pth);
#endif
#undef TEST

	return 0;
}

#else

/* !__GNUC__; Can't test this without GNU C */
int main(void)
{
	puts("Non-GCC compiler; skipping test.");
	return 0;
}

#endif
