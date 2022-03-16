#include <string.h>

#include <csnip/x.h>

char* csnip_x_strtok_r_imp(char* str, const char* delim, char** saveptr)
{
	char* p = (str ? str : *saveptr);
	if (p == NULL) {
		/* This can happen if the last token was found in the
		 * previous call
		 */
		return NULL;
	}

	/* Skip initial delimiters */
	while (*p && strchr(delim, *p) != NULL)
		++p;
	if (*p == '\0')
		return NULL;

	/* Find the next delimiter */
	char* q = p;
	while (*q && strchr(delim, *q) == NULL)
		++q;

	/* Replace delimiter with NUL char */
	if (*q == '\0') {
		*saveptr = NULL;
	} else {
		*q = '\0';
		*saveptr = q + 1;
	}

	return p;
}
