/* Smoke test for mem_* module.  This just tests very basic
 * functionality, in particular that code with those modules compiles.
 */

#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>

struct someStruct {
	float f;
	int z;
};

static int ftest(void)
{
	int* p;
	mem_Alloc(1, p, _);
	*p = 42;
	printf("*p = %d\n", *p);

	long* q;
	mem_Alloc0(12, q, _);
	long sum = 0;
	for (int i = 0; i < 12; ++i) {
		sum += q[i];
	}
	printf("sum(zeroinit) = %ld\n", sum);
	if (sum != 0) {
		fprintf(stderr, "Error:  Not zero as expected.\n");
		return 0;
	}
	mem_Free(q);

	struct someStruct* U;
	mem_AlignedAlloc(3, 64, U, _);
	mem_AlignedFree(U);

	mem_Realloc(11, p, _);
	printf("*p = %d\n", *p);
	if (*p != 42) {
		fprintf(stderr, "Error:  Not 42 as expected.\n");
		return 0;
	}
	p[10] = 999;
	mem_Free(p);

	return 1;
}

static int gtest(void)
{
	int* p;
	if (mem_Allocx(1, p) != 0) {
		fprintf(stderr, "Error: Malloc failed.\n");
		return 0;
	}
	*p = 42;
	printf("*p = %d\n", *p);

	long* q;
	if (mem_Alloc0x(12, q) != 0) {
		fprintf(stderr, "Error:  Malloc failure.\n");
		return 0;
	}
	long sum = 0;
	for (int i = 0; i < 12; ++i) {
		sum += q[i];
	}
	printf("sum(zeroinit) = %ld\n", sum);
	if (sum != 0) {
		fprintf(stderr, "Error:  Not zero as expected.\n");
		return 0;
	}
	mem_Free(q);

#if 0
	struct someStruct* U;
	if (mem_AlignedAllocx(3, 64, U) != 0) {
		fprintf(stderr, "Error:  Alligned allocation failure.\n");
		return 0;
	}
	mem_AlignedFree(U);
#endif

	int err = 0;
	mem_Realloc(11, p, err);
	if (err != 0) {
		fprintf(stderr, "Error:  Realloc failed\n");
		return 0;
	}
	printf("*p = %d\n", *p);
	if (*p != 42) {
		fprintf(stderr, "Error:  Not 42 as expected.\n");
		return 0;
	}
	p[10] = 999;
	mem_Free(p);

	return 1;
}

int main(int argc, char** argv)
{
	if (!ftest())
		return 1;
	if (!gtest())
		return 1;
	return 0;
}
