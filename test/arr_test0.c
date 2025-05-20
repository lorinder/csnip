#include <stdio.h>
#include <stdlib.h>

#include <csnip/arr.h>

struct IntArray {
	int* el;
	int n;
	int cap;
};

CSNIP_ARR_DECL_FUNCS(, IntArray_, int, args(struct IntArray* A))
CSNIP_ARR_DEF_FUNCS(, IntArray_, int, args(struct IntArray* A),
			A->el, A->n, A->cap, _)

int main()
{
	printf("Experiments with IntArray types.\n");
	struct IntArray Ax;
	IntArray_init(&Ax, 16);
	for (int i = 0; i < 128; ++i) {
		IntArray_push(&Ax, 3*i - 128);
	}
	printf("Array contains %d elements.\n", Ax.n);
	printf("Array has capacity %d.\n", Ax.cap);
	const int N = 3;
	for(int i = 0; i < N; ++i) {
		printf("[%d] %d\n", i, Ax.el[i]);
	}
	puts("...");
	for(int i = 0; i < N; ++i) {
		printf("[%d] %d\n", Ax.n - i - 1, Ax.el[Ax.n - i - 1]);
	}
	IntArray_deinit(&Ax);
	return 0;
}
