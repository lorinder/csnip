#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>

/** @file getopt.c
 *  @brief Getopt example for comparison.
 *
 */

/** \cond */
static void usage()
{
	printf("Tool to compare to getopt.\n\n");
	printf("  -h   display help and exit.\n");
	printf("  -i   set integer\n");
	printf("  -l   set long\n");
	printf("  -u   set unsigned long\n");
	printf("  -s   set string\n");
	exit(0);
}

int main(int argc, char** argv)
{
	int i = 0;
	long l = 1;
	unsigned long ul = 2;
	const char* str = "unset";

	int c;
	while ((c = getopt(argc, argv, "hi:l:u:s:")) != -1) {
		switch (c) {
		case 'h':
			usage();
		case 'i':
			i = atoi(optarg);
			break;
		case 'l':
			l = strtol(optarg, NULL, 0);
			break;
		case 'u':
			ul = strtoul(optarg, NULL, 0);
			break;
		case 's':
			str = optarg;
			break;
		case '?':
			return 1;
		};
	}

	/* Display result */
	printf("Done with argument processing.\n");
	printf("Got i = %d, l = %ld, ul = %lu, str = \"%s\"\n", i, l, ul, str);

	return 0;
}
/** \endcond */
