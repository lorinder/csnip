#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>

/** @file getopt.c
 *  @brief Getopt example for comparison.
 *
 */

/** \cond */
static void usage()
{
	puts("Tool to compare to getopt.\n\n"
		"  -h   display help and exit.\n"
		"  -i   set integer\n"
		"  -l   set long\n"
		"  -u   set unsigned long\n"
		"  -s   set string");
	exit(0);
}

int main(int argc, char** argv)
{
	int i = 0;
	long l = 1;
	unsigned long ul = 2;
	const char* str = "unset";

	int c;
	while ((c = x_getopt(argc, argv, "hi:l:u:s:")) != -1) {
		switch (c) {
		case 'h':
			usage();
		case 'i':
			i = atoi(x_optarg);
			break;
		case 'l':
			l = strtol(x_optarg, NULL, 0);
			break;
		case 'u':
			ul = strtoul(x_optarg, NULL, 0);
			break;
		case 's':
			str = x_optarg;
			break;
		case '?':
			return 1;
		};
	}

	/* Display result */
	printf("Done with argument processing.\n");
	printf("Got i = %d, l = %ld, ul = %lu, str = \"%s\"\n", i, l, ul, str);
	if (x_optind < argc) {
		printf("Positional arguments:");
		for (int i = x_optind; i < argc; ++i)
			printf(" \"%s\"", argv[i]);
		putchar('\n');
	}

	return 0;
}
/** \endcond */
