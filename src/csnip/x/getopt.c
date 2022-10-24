#include <stdio.h>
#include <string.h>

char* csnip_x_optarg_imp = NULL;
int csnip_x_optind_imp = 1;
int csnip_x_optopt_imp = 0;
int csnip_x_opterr_imp = 1;

static int idx_in_opt = 1;

#define optarg		csnip_x_optarg_imp
#define optind		csnip_x_optind_imp
#define optopt		csnip_x_optopt_imp
#define opterr		csnip_x_opterr_imp

int csnip_x_getopt_imp(int argc, char* argv[], const char* optstring)
{
	/* Check termination conditions */
	if (argv[optind] == NULL
	    || *argv[optind] != '-'
	    || strcmp(argv[optind], "-") == 0)
	{
		return -1;
	}
	if (strcmp(argv[optind], "--") == 0) {
		++optind;
		return -1;
	}

	/* Check for initial colon */
	const char* pos = optstring;
	int colon_first = 0;
	if (*pos == ':') {
		colon_first = 1;
		++pos;
	}

	/* Find option */
	const char key = argv[optind][idx_in_opt];
	while (*pos && *pos != key) {
		pos += (pos[1] == ':' ? 2 : 1);
	}

	/* Option not found -> error */
	if (*pos == '\0') {
		if (opterr != 0) {
			fprintf(stderr, "%s:  Unknown option -%c\n",
			  argv[0], key);
		}
		optopt = key;
		if (argv[optind][idx_in_opt + 1] != '\0') {
			++idx_in_opt;
		} else {
			++optind;
			idx_in_opt = 1;
		}
		return '?';
	}

	/* Process option */
	if (pos[1] == ':') {
		/* Process option with argument */
		if (argv[optind][idx_in_opt + 1] == '\0') {
			/* Argument space-separated from option char */
			idx_in_opt = 1;
			optarg = argv[++optind];
			if (optarg == NULL) {
				optopt = key;
				if (colon_first)
					return ':';
				else if (opterr != 0) {
					fprintf(stderr, "%s:  Option -%c "
					  "requires an argument.\n",
					  argv[0], key);
				}
				return '?';
			}

			++optind;
			return key;
		}

		/* Process argument directly attached to option */
		optarg = &argv[optind][idx_in_opt + 1];
		idx_in_opt = 1;
		++optind;
	}

	/* Process option without argument */
	if (argv[optind][++idx_in_opt] == '\0') {
		++optind;
		idx_in_opt = 1;
	}
	return key;
}

