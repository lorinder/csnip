#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>

/* Macros to set the actual version of getopt used */
#if 1
#  define my_getopt	csnip_x_getopt_imp
#  define my_optarg	csnip_x_optarg_imp
#  define my_optind	csnip_x_optind_imp
#  define my_optopt	csnip_x_optopt_imp
#else
#  define my_getopt	csnip_x_getopt
#  define my_optarg	csnip_x_optarg
#  define my_optind	csnip_x_optind
#  define my_optopt	csnip_x_optopt
#endif

#define FL_OPTOPT	0x1
#define FL_OPTARG	0x2

typedef struct {
	int retval;
	int optind;
	unsigned flags;

	int optopt;
	char* optarg;
} getopt_result_s;

typedef struct {
	int argc;
	char** argv;
	const char* optstring;
} getopt_stimulus_s;

typedef struct {
	getopt_stimulus_s stim;
	getopt_result_s* results;
} getopt_instance_s;

getopt_instance_s iarr[] = {
  /* Simple hand coded example */
  { .stim = { 2, (char*[]){ "./test", "-x", NULL }, "x" },
    .results = (getopt_result_s[]){
      { 'x', 2 },
      { -1, 2 },
    },
  },

  /* A few samples */
  { .stim = { 5, (char*[]){ "hey", "-d", "-c", "-b", "-a", NULL, }, "abcd" },
    .results = (getopt_result_s[]){
		{ 'd', 2},
		{ 'c', 3},
		{ 'b', 4},
		{ 'a', 5},
		{ -1, 5},
    },
  },
  { .stim = { 5, (char*[]){ "hey", "-d", "-c", "-b", "-a", NULL, }, "abc:d" },
    .results = (getopt_result_s[]){
		{ 'd', 2},
		{ 'c', 4, FL_OPTARG, .optarg="-b", },
		{ 'a', 5},
		{ -1, 5},
    },
  },
  { .stim = { 3, (char*[]){ "jesus", "-b", "-a", NULL, }, ":a" },
    .results = (getopt_result_s[]){
		{ '?', 2, FL_OPTOPT, .optopt='b', },
		{ 'a', 3},
		{ -1, 3},
    },
  },
  { .stim = { 2, (char*[]){ "joe", "-x", NULL, }, ":x:" },
    .results = (getopt_result_s[]){
		{ ':', 2, FL_OPTOPT, .optopt='x', },
		{ -1, 2},
    },
  },
  { .stim = { 5, (char*[]){ "joe", "-x", "something", "--", "args", NULL, }, ":x:" },
    .results = (getopt_result_s[]){
		{ 'x', 3, FL_OPTARG, .optarg="something", },
		{ -1, 4},
    },
  },
  { .stim = { 6, (char*[]){ "joe", "-b", "something", "-a", "else", "cmdargs", NULL, }, ":a:b:" },
    .results = (getopt_result_s[]){
		{ 'b', 3, FL_OPTARG, .optarg="something", },
		{ 'a', 5, FL_OPTARG, .optarg="else", },
		{ -1, 5},
    },
  },
  { .stim = { 2, (char*[]){ "multiple_unknown", "-abc", NULL, }, "z" },
    .results = (getopt_result_s[]){
		{ '?', 1, FL_OPTOPT, .optopt='a', },
		{ '?', 1, FL_OPTOPT, .optopt='b', },
		{ '?', 2, FL_OPTOPT, .optopt='c', },
		{ -1, 2},
    },
  },
  { .stim = { 5, (char*[]){ "multiple_unknown2", "-a", "-b", "-cde", "-f", NULL, }, ":z" },
    .results = (getopt_result_s[]){
		{ '?', 2, FL_OPTOPT, .optopt='a', },
		{ '?', 3, FL_OPTOPT, .optopt='b', },
		{ '?', 3, FL_OPTOPT, .optopt='c', },
		{ '?', 3, FL_OPTOPT, .optopt='d', },
		{ '?', 4, FL_OPTOPT, .optopt='e', },
		{ '?', 5, FL_OPTOPT, .optopt='f', },
		{ -1, 5},
    },
  },

  /* More curious cases */
  { .stim = { 3, (char*[]){ "joe", "-x", "--", NULL, }, ":x:" },
    .results = (getopt_result_s[]){
		{ 'x', 3, FL_OPTARG, .optarg="--", },
		{ -1, 3},
    },
  },

  /* sentinel */
  { .stim = { 0 } },
};

static bool check_instance(const getopt_instance_s* inst)
{
	/* Reset */
	my_optind = 1;

	/* Describe stimulus */
	printf("Testcase, argv:\n");
	for (int i = 0; i < inst->stim.argc; ++i) {
		printf("  [%2d] \"%s\"\n", i, inst->stim.argv[i]);
	}

	/* Check results in sequence */
	bool is_last = false;
	int i = 0;
	do {
		/* getopt() */
		const int retval = my_getopt(inst->stim.argc,
				inst->stim.argv,
				inst->stim.optstring);

		/* compare */
		const getopt_result_s* rexp = &inst->results[i];
		if (rexp->retval == -1 || retval == -1)
			is_last = true;
		if (rexp->retval != retval) {
			printf("  [%d] mismatched return value, "
			  "expected %d ('%c'), got %d ('%c')\n",
			  i,
			  rexp->retval,
			  (isprint(rexp->retval) ? rexp->retval : '?'),
			  retval,
			  (isprint(retval) ? retval : '?'));
			return false;
		}
		if (rexp->optind != my_optind) {
			printf("  [%d] mismatched optind, "
			  "expected %d, got %d\n",
			  i,
			  rexp->optind, my_optind);
			return false;
		}
		if ((rexp->flags & FL_OPTOPT) && rexp->optopt != my_optopt) {
			printf("  [%d] mismatched optopt, "
			  "expected '%c', got '%c'\n",
			  i,
			  rexp->optopt, my_optopt);
			return false;
		}
		if ((rexp->flags & FL_OPTARG)
			&& strcmp(rexp->optarg, my_optarg) != 0)
		{
			printf("  [%d] mismatched optarg, "
			  "expected \"%s\", got \"%s\"\n",
			  i,
			  rexp->optarg, my_optarg);
			return false;
		}

		/* next */
		++i;
	} while(!is_last);

	return true;
}

static void cquotechari(int c, char** destp)
{
	char* p = *destp;
	if (c == '\n') {
		*p++ = '\\', *p++ = 'n';
	} else if (c == '\b') {
		*p++ = '\\', *p++ = 'b';
	} else if (c == '\0') {
		*p++ = '\\', *p++ = '0';
	} else if (isprint(c)) {
		*p++ = c;
	} else {
		*p++ = '\\', *p++ = 'x';
		snprintf(p, 3, "%02x", c);
		p += 2;
	}
	*destp = p;
}

static char* cquotechar(int c)
{
	static char buf[16];
	char* p = buf;
	if (c < 0) {
		snprintf(buf, sizeof buf, "%d", c);
		return buf;
	} else {
		*p++ = '\'';
		cquotechari(c, &p);
		*p++ = '\'';
		*p = '\0';
		return buf;
	}
}



static const char* cquote(const char* str)
{
	if (str == NULL)
		return "NULL";
	static char buf[256];
	char* p = buf;
	*p++ = '"';
	for (const char* q = str; *q; ++q) {
		cquotechari(*q, &p);
	}
	*p++ = '"';
	*p = '\0';
	return buf;
}

static void build_example(int argc, char** argv, char* optstr)
{
	/* Write out the stimulus */
	printf("  { .stim = { %d, (char*[]){ ", argc);
	for (int i = 0; i <= argc; ++i) {
		printf("%s, ", cquote(argv[i]));
	}
	printf("}, %s },\n", cquote(optstr));

	/* Create results, one by one */
	printf("    .results = (getopt_result_s[]){\n");
	int c;
	do {
		c = my_getopt(argc, argv, optstr);

		/* Print the return value and resulting optind value */
		printf("\t\t{ %s, %d",
		  cquotechar(c),
		  my_optind);

		/* Figure out the flags */
		int flags = 0;
		char flagstr[32] = { 0 };
		if (c == '?' || c == ':') {
			flags |= FL_OPTOPT;
			strcpy(flagstr, "FL_OPTOPT");
		}

		char* p = strchr(optstr, c);
		if (p && p[1] == ':') {
			flags |= FL_OPTARG;
			if (*flagstr)
				strcat(flagstr, "|");
			strcat(flagstr, "FL_OPTARG");
		}

		/* Print the flags & optional global return values */
		if (flags) {
			printf(", %s, ", flagstr);
			if (flags & FL_OPTOPT) {
				printf(".optopt=%s, ",
				  cquotechar(my_optopt));
			}
			if (flags & FL_OPTARG) {
				printf(".optarg=%s, ",
				  cquote(my_optarg));
			}
		}
		printf("},\n");
	} while(c != -1);
	puts("    },\n  },");
}

int main(int argc, char** argv)
{
	if (argc >= 2) {
		/* Create an example */
		char* optstr = argv[1];
		build_example(argc - 2, argv + 2, optstr);
	} else {
		/* Run getopt() testing */
		puts("getopt() test");

		for (int i = 0; iarr[i].stim.argc > 0; ++i) {
			getopt_instance_s* inst = &iarr[i];
			if (!check_instance(inst)) {
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}
