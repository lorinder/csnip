#include <stdio.h>

#include <csnip/fmt.h>

int main(int argc, char** argv)
{
	char* fmtstr = "${greeting} there, ${name}.";
	char* kv_pairs[] = {
		"greeting",	"Hello",
		"name",		"world",
		NULL		/* Sentinel */
	};

	/* Format */
	char* output_str = NULL;
	int err = 0;
	csnip_fmt_Str(fmtstr,				/* format string */
		'$',							/* key character */
		s, e, v, p,						/* dummy vars */
		csnip_fmt_ScanShStyle(s, e, p),	/* scan_key */
		csnip_fmt_ListMatch(s, e, v,
		  kv_pairs),					/* get_val */
		/* unused */,					/* free_val */
		output_str,						/* result variable */
		err);							/* error return */

	/* Print result */
	if (err != 0)
		printf("Formatting error: %d\n", err);
	printf("Formatted string: \"%s\"\n",
		(output_str ? output_str : "(null)"));
	free(output_str);

	return 0;
}

// vim:ts=4
