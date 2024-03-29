#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/err.h>
#include <csnip/clopts.h>

int main(int argc, char** argv)
{
	clopts opts = {
	  .description = "Testing tool for clopts library."
	};
	clopts_add_defaults(&opts);

	/* Parse the command line args */
	int i = 0;
	long l = 1;
	unsigned long ul = 2;
	const char* str = "unset";
	_Bool flag = 0;
	clopts_Addvar(&opts, 'i', "int", "int argument", &i, _);
	clopts_Addvar(&opts, 'l', "long", "long int argument", &l, _);
	clopts_Addvar(&opts, 'u', "ulong", "unsigned long argument", &ul, _);
	clopts_Addvar(&opts, 's', "string", "string argument", &str, _);
	clopts_Addflag(&opts, 'f', "flag", "flag", &flag, _);
	int poffs;
	int err = clopts_process(&opts, argc - 1, argv + 1, &poffs, true);
	if (err != 0) {
		char buf[128];
		err_str(err, buf, sizeof(buf));
		printf("Error from clopts_process: %s\n", buf);
		return 1;
	}

	/* Display result */
	printf("Done with argument processing.\n");
	printf("Got i = %d, l = %ld, ul = %lu, str = \"%s\", flag = %s\n",
	  i, l, ul, str, (flag ? "true" : "false"));
	if (++poffs < argc) {
		printf("Positional arguments:");
		for (int i = poffs; i < argc; ++i)
			printf(" \"%s\"", argv[i]);
		putchar('\n');
	}

	return 0;
}
