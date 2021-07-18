#include <string.h>

#include <csnip/x.h>

char* csnip_x_strdup(const char* s)
{
#ifdef _MSC_VER
	return _strdup(s);
#else
	return strdup(s);
#endif
}
