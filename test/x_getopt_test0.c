#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>

#define FL_OPTIND	0x1
#define FL_OPTOPT	0x2
#define FL_OPTARG	0x4

typedef struct {
	int retval;

	unsigned flags;
	int optind;
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
  { .stim = { 2, (char*[]){ "./test", "-x", NULL }, "x" },
    .results = (getopt_result_s[]){
      { .retval = 'x', .flags = 0 },
      { .retval = -1 },
    },
  },

  /* sentinel */
  { .stim = { 0 } },
};

static bool check_instance(const getopt_instance_s* inst)
{
	/* Reset */
	optind = 1;

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
		const int retval = getopt(inst->stim.argc,
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
		if ((rexp->flags & FL_OPTIND) && rexp->optind != optind) {
			printf("  [%d] mismatched optind, "
			  "expected %d, got %d\n",
			  i,
			  rexp->optind, optind);
			return false;
		}
		if ((rexp->flags & FL_OPTOPT) && rexp->optopt != optopt) {
			printf("  [%d] mismatched optopt, "
			  "expected '%c', got '%c'\n",
			  i,
			  rexp->optopt, optopt);
			return false;
		}
		if ((rexp->flags & FL_OPTARG)
			&& strcmp(rexp->optarg, optarg) != 0)
		{
			printf("  [%d] mismatched optarg, "
			  "expected \"%s\", got \"%s\"\n",
			  i,
			  rexp->optarg, optarg);
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
	printf("}, %s }\n", cquote(optstr));

	/* Create results, one by one */
	printf("    .results = (getopt_result_s[]){\n");
	int c;
	do {
		c = getopt(argc, argv, optstr);
		printf("\t\t{ %s, %s, %d, ",
		  cquotechar(c),
		  "FL_OPTIND|FL_OPTOPT|FL_OPTARG",
		  optind);
		printf("%s, %s },\n",
		  cquotechar(optopt),
		  cquote(optarg));
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
