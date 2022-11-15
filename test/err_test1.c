#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>

int main(void)
{
	char* a = "XXX";
	// XXX:  This is a very broken test.  Always has been.  Remove
	// it or something.
	mem_Alloc(1024, a, error_ignore);
	printf("Pointer: %p\n", (void*)a);

	return 0;
}
