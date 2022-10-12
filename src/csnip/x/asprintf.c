#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <csnip/x.h>

int csnip_x_asprintf_imp(char** strp, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	const int r = csnip_x_vasprintf_imp(strp, format, ap);
	va_end(ap);
	return r;
}

int csnip_x_vasprintf_imp(char** strp, const char* format, va_list ap)
{
	va_list aq;
	va_copy(aq, ap);

	/* Determine the size, get buffer */
	const int sz = vsnprintf(NULL, 0, format, ap);
	*strp = malloc(sz + 1);
	if (*strp == NULL)
		return -1;

	/* Write */
	vsnprintf(*strp, sz + 1, format, aq);
	va_end(aq);

	return sz;
}
