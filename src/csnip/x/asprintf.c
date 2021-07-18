#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>

#include <csnip/x.h>

int csnip_x_asprintf(char** strp, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	const int r = csnip_x_vasprintf(strp, format, ap);
	va_end(ap);
	return r;
}

int csnip_x_vasprintf(char** strp, const char* format, va_list ap)
{
	const int r = vasprintf(strp, format, ap);
	if (r == -1 && strp) {
		*strp = NULL;
	}
	return r;
}
