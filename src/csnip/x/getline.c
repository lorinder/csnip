#include <csnip/x.h>

csnip_x_ssize_t csnip_x_getline(char** lineptr,
				size_t* n,
				FILE* fp)
{
	return csnip_x_getdelim_imp(lineptr, n, '\n', fp);
}
