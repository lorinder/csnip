#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSNIP_SHORT_NAMES
#include <csnip/fmt.h>

static void verify(const char* got, const char* expected)
{
	if (strcmp(got, expected) == 0)
		return;
	fprintf(stderr, "FAILED:\n"
		"   Got:      \"%s\"\n"
		"   Expected: \"%s\"\n",
		got, expected);
	exit(1);
}

int main(int argc, char** argv)
{
	const char* fmtstr1 = "Hi, @name@, how are you? @response@";
	const char* fmtstr2 = "Now, $a+$b=${c}s";
	if (argc > 1)
		fmtstr1 = argv[1];
	if (argc > 2)
		fmtstr2 = argv[2];

	/* Formatting @-style */
	const char* kv_pairs1[] = {
		"name",		"Moritz",
		"response",	"Great!",
		NULL, /* sentinel */
	};
	char* r = NULL;
	fmt_Str(fmtstr1,				/* Format string */
		'@',					/* Key char */
		s, e, val, next,			/* Dummy vars */
		fmt_ScanToChar(s, e, next, '@'),	/* Scan key */
		fmt_ListMatch(s, e, val, kv_pairs1),	/* Get value */
		/* Nop */,				/* Free */
		r,					/* Output */
		_);					/* Error return */
	printf("format string 1: \"%s\"\n", fmtstr1);
	printf("output:          \"%s\"\n", r);
	verify(r, "Hi, Moritz, how are you? Great!");
	free(r);

	/* Formatting shell style */
	const char* kv_pairs2[] = {
		  "a",		"Letter A",
		  "b",		"B",
		  "c",		"cee",
		  NULL /* sentinel */
	};
	r = NULL;
	fmt_Str(fmtstr2,
		'$',
		s, e, val, next,
		fmt_ScanShStyle(s, e, next),		/* Scan key */
		fmt_ListMatch(s, e, val, kv_pairs2),	/* Get value */
		/* Nop */,				/* Free */
		r,					/* Output */
		_);					/* Error return */
	printf("format string 2: \"%s\"\n", fmtstr2);
	printf("output:          \"%s\"\n", r);
	verify(r, "Now, Letter A+B=cees");
	free(r);

	return 0;
}
