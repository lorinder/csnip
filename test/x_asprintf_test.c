#include <string.h>
#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>

int main(void)
{
	const char* expected = "Hi, there, 12";
	char* x;
	if (x_asprintf(&x, "Hi, there, %d", 12) == -1) {
		return 1;
	}
	if (strcmp(x, expected) != 0) {
		fprintf(stderr, "Error: Got \"%s\" instead of "
			"the expected \"%s\".\n", x, expected);
		return 1;
	} else {
		puts("ok, passed.");
	}

	return 0;
}
