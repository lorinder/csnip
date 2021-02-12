#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CSNIP_SHORT_NAMES
#include <csnip/clopts.h>
#include <csnip/mem.h>
#include <csnip/util.h>

typedef struct {
	int iarg;		/**< Integer argument */
	float farg;		/**< Float argument */
	double darg;		/**< Double argument */
	const char* strarg;	/**< String argument */

	int npargs;		/**< Number of positional arguments */
	char** pargs;		/**< The positional arguments */
} Outcome;

static bool check_outcomes_equal(const Outcome* a, const Outcome* b)
{
	/* Check supplied flags */
#define CHECK(n) do { \
	if (a->n != b->n) { \
		fprintf(stderr, "Error:  Outcome mismatch: %s\n", #n); \
		return false; \
	} } while (0)
	CHECK(iarg);
	CHECK(farg);
	CHECK(darg);
#undef CHECK
	if ((a->strarg == NULL) != (b->strarg == NULL)) {
		fprintf(stderr, "Error:  Outcome mismatch: strarg "
		  "set in expected and not obtained or vice versa.\n");
		return false;
	}
	if (a->strarg && strcmp(a->strarg, b->strarg) != 0)
	{
		fprintf(stderr, "Error:  Outcome mismatch: strarg\n");
		return false;
	}

	/* Check the positional arguments */
	if (a->npargs != b->npargs) {
		fprintf(stderr, "Error:  Outcome mismatch: Number of "
		  "positional arguments\n");
		return false;
	}
	for (int i = 0; i < a->npargs; ++i) {
		if (strcmp(a->pargs[i], b->pargs[i]) != 0) {
			fprintf(stderr, "Error:  Outcome mismatch: "
			  "Positional argument %d\n", i);
			return false;
		}
	}

	return true;
}

typedef struct {
	/**	Human readable description. */
	char*	descr;

	/**	Work area for clopts. */
	Outcome work;

	/**	Expected outcome after the tester has run */
	Outcome expected;

	/**	Tester function.
	 *
	 *	Should fill in the work structure (which has been
	 *	appropriately initialized;  if the ->pargs member is
	 *	set, it is assumed to be assigned to a dynamically
	 *	allocated array.  All other pointers point to statically
	 *	allocated memory.
	 */
	bool (*tester)(Outcome* work, void* arg);

	/**	Special argument for the tester function. */
	void* arg;
} Testcase;

static bool args_tester1(Outcome* work, void* args_)
{
	char** args = args_;

	/* Find number of args */
	int nargs = 0;
	while (args[nargs])
		++nargs;

	/* Create and process clopts */
	clopts opts = { };
	clopts_Addvar(&opts, 'i', "int", "int arg", &work->iarg, _);
	clopts_Addvar(&opts, 'f', "float", "float arg", &work->farg, _);
	clopts_Addvar(&opts, 'd', "double", "double arg", &work->darg, _);
	clopts_Addvar(&opts, 's', "str", "string arg", &work->strarg, _);
	int poffs;
	if (clopts_process(&opts, nargs, args, &poffs, true) != 0)
		return false;

	/* Copy positional arguments if any */
	if (poffs < nargs) {
		work->npargs = nargs - poffs;
		mem_Alloc(work->npargs, work->pargs, _);
		Copy_n(args + poffs, work->npargs, work->pargs);
	}
	return true;
}

Testcase testcases[] = {
{ .descr = "Check parsers for -i, -f, -d, -s, and positional arguments",
  .work = { },
  .expected = { .iarg = 5,
		.farg = 2.0,
		.darg = -3.1,
		.strarg = "hi there",
		.npargs = 2,
		.pargs = (char*[]){ "pos1", "pos2" },
  },
  .tester = args_tester1,
  .arg = (char*[]){ "-i", "5", "-f", "2.0", "-d", "-3.1", "-s", "hi there",
			"pos1", "pos2", NULL },
},
{ .descr = "Check that the second argument counts if duplicates given",
  .work = { },
  .expected = {	.iarg = 27,
		.farg = 3.141,
		.darg = 2.71,
		.strarg = "second string",
  },
  .tester = args_tester1,
  .arg = (char*[]){ "-i", "99", "-f", "99", "-d", "99", "-s", "first string",
		    "-i", "27", "-f", "3.141", "-d", "2.71",
		    "-s", "second string", NULL },
},
{ .descr = "Check arguments directly adjoined to the flags",
  .work = { },
  .expected = {	.iarg = 19,
		.strarg = "Yo",
  },
  .tester = args_tester1,
  .arg = (char*[]) { "-i19", "-sYo", NULL },
},
{ .descr = "Check that -- starts positional args",
  .work = { },
  .expected = { .farg = 7,
		.npargs = 2,
		.pargs = (char*[]){ "-d", "12" }
  },
  .tester = args_tester1,
  .arg = (char*[]) { "-f7", "--", "-d", "12", NULL }
},
{ .descr = "Check long form args, = syntax",
  .work = { },
  .expected = { .iarg = 1,
		.farg = 2,
		.darg = 3,
		.strarg = "4",
  },
  .tester = args_tester1,
  .arg = (char*[]) { "--int=1", "--float=2", "--double=3", "--str=4", NULL },
},
{ .descr = "Check long form args, space syntax",
  .work = { },
  .expected = {	.iarg = 72,
		.farg = 71,
		.darg = 70,
		.strarg = "Jonathan",
		.npargs = 1,
		.pargs = (char*[]) { "Horse" },
  },
  .tester = args_tester1,
  .arg = (char*[]) { "--int", "72", "--float", "71", "--double", "70",
		"--str", "Jonathan", "Horse", NULL },
},
{ .descr = "Check that - alone counts as positional arg",
  .work = { },
  .expected = {	.npargs = 1,
		.pargs = (char*[]) { "-" },
  },
  .tester = args_tester1,
  .arg = (char*[]) { "-", NULL },
},
};

int main(int argc, char** argv)
{
	bool success = true;
	for (int i = 0; i < Static_len(testcases); ++i) {
		Testcase* T = &testcases[i];
		printf("Running test case %d: \"%s\"\n", i, T->descr);
		if (!(*T->tester)(&T->work, T->arg)) {
			puts("-> FAILED. Tester returned an error.");
			success = false;
		}
		const bool eq = check_outcomes_equal(&T->work,
						&T->expected);
		if (!eq) {
			puts("-> FAILED. Outcome not matching expectations.");
			success = false;
		}
		if (T->work.pargs) {
			mem_Free(T->work.pargs);
		}
		puts("-> pass.");
	}

	printf("Summary: %s\n", (success ? "pass" : "FAIL"));
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
