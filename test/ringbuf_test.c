#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/err.h>
#include <csnip/util.h>
#include <csnip/ringbuf.h>

/* Tests for the macros */

#define DEF_TEST_FUNC(name, push_macro, pop_macro, reverse) \
	static void name() \
	{ \
		const int samplevals[] = { 11, 453, 712, 991, 13, 19, 2 }; \
		int a[7]; \
		int head, len; \
		const int N = Static_len(a); \
		\
		printf("Test func: %s, push=%s, pop=%s\n", \
			  #name, #push_macro, #pop_macro); \
		\
		ringbuf_Init(head, len, N); \
		for (int j = 0; j <= N; ++j) { \
			\
			/* Insert values */ \
			for (int i = 0; i < j; ++i) { \
				push_macro(head, len, N, a, \
				  samplevals[i], _); \
			} \
			\
			/* Read them off */ \
			for (int i = 0; i < j; ++i) { \
				int r; \
				pop_macro(head, len, N, a, r, _); \
				const int idx = (reverse ? j - i - 1 : i); \
				if (r != samplevals[idx]) { \
					fprintf(stderr, "Error:  Mismatch. " \
					  "Expected %d, got %d\n", \
					  samplevals[idx], r); \
					exit(1); \
				} \
			} \
			\
			/* Check for underflow */ \
			int r, err = 0; \
			pop_macro(head, len, N, a, r, err); \
			if (err != csnip_err_UNDERFLOW) { \
				fprintf(stderr, "Error: Underflow not signaled " \
				  "as expected.\n"); \
				exit(1); \
			} \
			(void)r; \
		} \
	}


DEF_TEST_FUNC(test_pushhead_pophead, ringbuf_PushHead, ringbuf_PopHead, 1)
DEF_TEST_FUNC(test_pushhead_poptail, ringbuf_PushHead, ringbuf_PopTail, 0)
DEF_TEST_FUNC(test_pushtail_pophead, ringbuf_PushTail, ringbuf_PopHead, 0)
DEF_TEST_FUNC(test_pushtail_poptail, ringbuf_PushTail, ringbuf_PopTail, 1)

#undef DEF_TEST_FUNC

/* Tests for the CSNIP_RINGBUF_DEF* pieces */

typedef struct {
	int elem[64];
	int head, len;
} myRbType;

CSNIP_RINGBUF_DECL_IDX_FUNCS(static cext_unused, RB_, args(myRbType*, int*))
CSNIP_RINGBUF_DEF_IDX_FUNCS(static, RB_, args(myRbType* rb, int* err),
	rb->head, rb->len, Static_len(rb->elem), *err)
CSNIP_RINGBUF_DECL_VAL_FUNCS(static cext_unused, RB_, int, args(myRbType*, int*))
CSNIP_RINGBUF_DEF_VAL_FUNCS(static, RB_, int, args(myRbType* rb, int* err),
	rb->head, rb->len, Static_len(rb->elem), rb->elem, *err)

int main(int argc, char** argv)
{
	test_pushhead_poptail();
	test_pushhead_pophead();
	test_pushtail_pophead();
	test_pushtail_poptail();
	return 0;
}
