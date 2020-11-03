/* Smoke test for mem_* module.  This just tests very basic
 * functionality, in particular that code with those modules compiles.
 */

#include <cstdio>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>

struct someStruct {
	float f;
	int z;
};

int main(int argc, char** argv)
{
	int* p;
	mem_Alloc(1, p, _);
	*p = 42;
	std::printf("*p = %d\n", *p);

	long* q;
	mem_Alloc0(12, q, _);
	long sum = 0;
	for (int i = 0; i < 12; ++i) {
		sum += q[i];
	}
	std::printf("sum(zeroinit) = %ld\n", sum);
	if (sum != 0) {
		std::fprintf(stderr, "Error:  Not zero as expected.\n");
	}
	mem_Free(q);

	struct someStruct* U;
	mem_AlignedAlloc(3, 64, U, _);
	mem_AlignedFree(U);

	mem_Realloc(11, p, _);
	std::printf("*p = %d\n", *p);
	if (*p != 42) {
		std::fprintf(stderr, "Error:  Not 42 as expected.\n");
	}
	p[10] = 999;
	mem_Free(p);

	return 0;
}
