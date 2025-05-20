#include <stdio.h>

#include <csnip/cext.h>

csnip_cext_unused static int f(void)
{
	return 0;
}

csnip_cext_export void g(void);

void g(void)
{
	puts("Exported function");
}

int main(void)
{
	puts("No runtime tests here -> pass.");
	return 0;
}
