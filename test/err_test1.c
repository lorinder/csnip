#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>

int main(void)
{
	char* a = "XXX";
	mem_Alloc(128ull * 1024 * 1024 * 1024, a, error_ignore);
	printf("Pointer: %p\n", (void*)a);

	return 0;
}
