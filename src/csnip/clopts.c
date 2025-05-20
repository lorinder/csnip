#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#define CSNIP_SHORT_NAMES
#include "clopts.h"

#include <csnip/arr.h>
#include <csnip/podtypes.h>
#include <csnip/preproc.h>
#include <csnip/util.h>

static int print_help(const clopts* opts,
			const clopts_optinfo* arginfo,
			const char* argval)
{
	printf("%s\n\n", opts->description);
	for (int i = 0; i < opts->n_optinfo; ++i) {

		const clopts_optinfo* const a = &opts->optinfo[i];
		char buf[64] = "";
		if (a->short_name && a->long_name) {
			snprintf(buf, sizeof(buf), "-%c, --%s",
			  a->short_name, a->long_name);
		} else if (a->short_name) {
			snprintf(buf, sizeof(buf), "-%c", a->short_name);
		} else if (a->long_name) {
			snprintf(buf, sizeof(buf), "--%s", a->long_name);
		}

		/* Print option with help string */
		printf("   %-20s %s\n", buf, a->description);
	}
	exit(0);
}

int csnip_clopts_add_defaults(csnip_clopts* opts)
{
	static const clopts_optinfo defaults[] = {
		{ .short_name = 'h',
		  .long_name = "help",
		  .description = "display help and exit",
		  .parser = print_help
		},
	};
	return csnip_clopts_add(opts, Static_len(defaults), defaults);
}

int csnip_clopts_add(csnip_clopts* opts,
			int n_optinfo,
			const csnip_clopts_optinfo* optinfo)
{
	for (int i = 0; i < n_optinfo; ++i) {
		int err = 0;
		csnip_arr_Push(opts->optinfo,
				opts->n_optinfo,
				opts->n_optinfo_cap,
				optinfo[i],
				err);
		if (err != 0) {
			return err;
		}
	}
	return 0;
}

static bool check_longopt_match(const char* optstr,
				const clopts_optinfo* I,
				const char** p_optarg)
{
	/* Exclude non-matches */
	if (I->long_name == NULL)
		return false;
	const size_t l = strlen(I->long_name);
	if (strncmp(optstr + 2, I->long_name, l) != 0)
		return false;
	if (optstr[l + 2] != '\0' &&
	    (optstr[l + 2] != '=' || !I->takes_val))
		return false;

	/* Option match, now set p_optarg if necessary */
	if (p_optarg) {
		*p_optarg = NULL;
		if (optstr[l + 2] == '=')
			*p_optarg = &optstr[l + 3];
	}
	return true;
}

/* Internal version of clopts_process() that does not clear memory. */
static int process_noclear(csnip_clopts* opts,
			int argc,
			char** argv,
			int* ret_pos_args)
{
	int i;
	for (i = 0; i < argc; ++i) {
		const char* optstr = argv[i];

		/* Exit processing on "-", "--" or argument starting
		 * without dash.
		 *
		 * In the case of "--", consume that argument first.
		 */
		if (*optstr != '-' || optstr[1] == '\0') {
			break;
		}
		if (strcmp(optstr, "--") == 0) {
			++i;
			break;
		}

		if (optstr[1] == '-') {
			/* Long option */
			const char* optarg = NULL;
			clopts_optinfo* I = NULL;
			int e;

			/* Find match */
			for (e = 0; e < opts->n_optinfo; ++e) {
				optarg = NULL;
				I = &opts->optinfo[e];
				if (check_longopt_match(optstr, I, &optarg)) {
					break;
				}
			}
			if (e == opts->n_optinfo) {
				fprintf(stderr, "Error:  Option `%s' "
				  "unknown.\n", optstr);
				return err_FORMAT;
			}

			/* Handle match */
			if (optarg == NULL && I->takes_val) {
				if (i + 1 >= argc) {
					fprintf(stderr, "Error:  Option --%s "
					  "takes value, but none given.\n",
					  I->long_name);
					return err_FORMAT;
				}
				optarg = argv[i + 1];
				++i;
			}

			/* Call handler */
			int rc = I->parser(opts, I, optarg);
			if (rc != 0)
				return rc;
		} else {
			/* Short options
			 *
			 * Process the string as a sequence of options.
			 */
			for (const char* p = optstr + 1; *p != '\0'; ++p) {

				/* Find the optinfo */
				clopts_optinfo* I = NULL;
				int e;
				for (e = 0; e < opts->n_optinfo; ++e) {
					I = &opts->optinfo[e];
					if (I->short_name == *p) {
						/* Opt found */
						break;
					}
				}
				if (e == opts->n_optinfo) {
					fprintf(stderr, "Error:  Option -%c "
					  "unknown.\n", *p);
					return err_FORMAT;
				}

				/* Find the option argument, if any */
				const char* optarg = NULL;
				bool finished = false;
				if (I->takes_val) {
					finished = true;
					if (p[1]) {
						optarg = p + 1;
					} else {
						// XXX: duplicate code.
						if (i + 1 >= argc) {
							fprintf(stderr, "Error:  Option --%s "
							  "takes value, but none given.\n",
							  I->long_name);
							return err_FORMAT;
						}
						optarg = argv[i + 1];
						++i;
					}
				}

				/* Call handler */
				int rc = I->parser(opts, I, optarg);
				if (rc != 0)
					return rc;

				/* Next */
				if (finished)
					break;
			}
		}
	}

	/* Assign positional args */
	if (ret_pos_args) {
		*ret_pos_args = i;
	} else {
		if (i < argc) {
			fprintf(stderr, "Error:  Unexpected positional "
			  "command line arguments.\n");
			return err_FORMAT;
		}
	}
	return 0;
}

int csnip_clopts_process(csnip_clopts* opts,
			int argc,
			char** argv,
			int* ret_pos_args,
			bool do_clear)
{
	const int rc = process_noclear(opts, argc, argv, ret_pos_args);
	if (do_clear)
		csnip_clopts_clear(opts);
	return rc;
}

void csnip_clopts_clear(csnip_clopts* opts)
{
	arr_Deinit(opts->optinfo, opts->n_optinfo, opts->n_optinfo_cap);
}

int csnip_clopts_parser_uchar(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval)
{
	if (argval == NULL)
		return err_UNEXPECTED_NULL;
	// XXX: It just may be that the user would rather want to get
	// the uchar parsed as an integer; think about this possibility.
	*(uchar*)optinfo->usr = *argval;
	return 0;
}

#define DEF_INT_PARSER(type, maxval) \
	int csnip_clopts_parser_##type( \
		const csnip_clopts* opts, \
		const csnip_clopts_optinfo* optinfo, \
		const char* argval) \
	{ \
		if (argval == NULL) { \
			fprintf(stderr, "Error:  Unexpected NULL " \
			  "pointer arg string as argval argument " \
			  "in %s.\n", __func__); \
			return err_UNEXPECTED_NULL; \
		} \
		char* endptr = NULL; \
		type* ret = (type*)optinfo->usr; \
		errno = 0; \
		ullong v = strtoull(argval, &endptr, 0); \
		if (errno != 0 && errno != ERANGE) \
			return err_ERRNO; \
		if (errno == ERANGE || maxval < v) { \
			fprintf(stderr, "Error:  Argument value " \
			 "\"%s\" out of range for storage type.\n", \
			 argval); \
			return err_RANGE; \
		} \
		if (endptr == argval) { \
			fprintf(stderr, "Error:  Can't convert \"%s\" " \
			  "to an integer.\n", argval); \
			return err_FORMAT; \
		} \
		if (*endptr != '\0') { \
			fprintf(stderr, "Error:  Trailing garbage " \
			  "after integer argument \"%s\".\n", \
			  argval); \
			return err_FORMAT; \
		} \
		*ret = (type)v; \
		return 0; \
	}

DEF_INT_PARSER(uint, UINT_MAX)
DEF_INT_PARSER(ulong, ULONG_MAX)
DEF_INT_PARSER(ullong, ULLONG_MAX)

#undef DEF_INT_PARSER

#define DEF_FLOAT_PARSER(type) \
	int csnip_clopts_parser_##type( \
		const csnip_clopts* opts, \
		const csnip_clopts_optinfo* optinfo, \
		const char* argval) \
	{ \
		if (argval == NULL) { \
			fprintf(stderr, "Error:  Unexpected NULL " \
			  "pointer arg string as argval argument " \
			  "in %s.\n", __func__); \
			return err_UNEXPECTED_NULL; \
		} \
		char* endptr = NULL; \
		type* ret = (type*)optinfo->usr; \
		errno = 0; \
		ldouble v = strtold(argval, &endptr); \
		if (errno != 0 && errno != ERANGE) \
			return err_ERRNO; \
		/* XXX: Check for overflow when converting down \
		   Currenly missing limit_Maxf macro for this. */ \
		if (errno == ERANGE) { \
			fprintf(stderr, "Error:  Argument value " \
			 "\"%s\" out of range for storage type.\n", \
			 argval); \
			return err_RANGE; \
		} \
		if (endptr == argval) { \
			fprintf(stderr, "Error:  Can't convert \"%s\" " \
			  "to a floating point value.\n", argval); \
			return err_FORMAT; \
		} \
		if (*endptr != '\0') { \
			fprintf(stderr, "Error:  Trailing garbage " \
			  "after floating point argument \"%s\".\n", \
			  argval); \
			return err_FORMAT; \
		} \
		*ret = (type)v; \
		return 0; \
	}

DEF_FLOAT_PARSER(float)
DEF_FLOAT_PARSER(double)
DEF_FLOAT_PARSER(ldouble)
#undef DEF_FLOAT_PARSER

int csnip_clopts_parser_pchar(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval)
{
	if (argval == NULL)
		return err_UNEXPECTED_NULL;
	const char** ret = (const char**)optinfo->usr;
	*ret = argval;
	return 0;
}

int csnip_clopts_flagparser_bool(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval)
{
	*(_Bool*)optinfo->usr = 1;
	return 0;
}
