/* This is pretty much a copy-paste of arr_test0.c with the minor top
 * changes
 */

#include <stdio.h>
#include <stdlib.h>

#include <csnip/arrt.h>
#include <csnip/cext.h>

CSNIP_ARRT_DEF_TYPE(IntArray, int)
CSNIP_ARRT_DEF_FUNCS(csnip_cext_unused static, IntArray_, IntArray, int)

int main()
{
	printf("Experiments with IntArray types.\n");
	IntArray Ax;
	IntArray_init(&Ax, NULL, 16);
	for (int i = 0; i < 128; ++i) {
		IntArray_push(&Ax, NULL, 3*i - 128);
	}
	printf("Array contains %zu elements.\n", Ax.n);
	printf("Array has capacity %zu.\n", Ax.cap);
	const int N = 3;
	for(int i = 0; i < N; ++i) {
		printf("[%d] %d\n", i, Ax.a[i]);
	}
	puts("...");
	for(int i = 0; i < N; ++i) {
		printf("[%zu] %d\n", Ax.n - i - 1, Ax.a[Ax.n - i - 1]);
	}
	IntArray_deinit(&Ax, NULL);
	return 0;
}
