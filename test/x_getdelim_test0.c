#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/util.h>
#include <csnip/x.h>

const char* file_content = "line 1\n"
	"\n"
	"01234567890123456789012345678901234567890123456789"
	"01234567890123456789012345678901234567890123456789\n"
	"no eol";

/* Lengths when the delimiter is '\n' */
const x_ssize_t lengths1[] = { 7, 1, 101, 6, -1 };

/* Lengths when the delimiter is '1' */
const x_ssize_t lengths2[] = { 6, 4, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 15, -1 };

static void create_temp_file(void)
{
	FILE* fp = fopen("tmpfile.txt", "w");
	fputs(file_content, fp);
	fclose(fp);
}

static bool check_getdelim(bool verbose,
			int delim,
			bool doImp,
			const x_ssize_t expected_lengths[])
{
	/* Process */
	printf("Test case: delim = %d, imp = %d\n", delim, (int)doImp);

	/* Read file */
	bool success = true;
	FILE* fp = fopen("tmpfile.txt", "r");

	char* lp = NULL;
	size_t n = 0;

	/* Read one by one */
	int i = 0;
	ssize_t m;
	const char* p = file_content;
	do {
		/* Read */
		m = (doImp ? x_getdelim_imp : x_getdelim)(&lp, &n, delim, fp);
		if (verbose) {
			printf("Read result, length = %zd", m);
			if (m > 0) {
				printf(", string = \"");
				for (x_ssize_t i = 0; lp[i]; ++i) {
					switch (lp[i]) {
					case '\n':	printf("\\n");	break;
					case '\r':	printf("\\r");	break;
					case '\\':	printf("\\");	break;
					default:
						putchar(isprint(lp[i]) ?
							lp[i] : '?');
					};
				}
				putchar('\"');
			}
			putchar('\n');
		}

		/* Check for match */
		bool lmatch = true, cmatch = true;
		if (m != expected_lengths[i])
			lmatch = false;
		if (m > 0 && strncmp(lp, p, Min(expected_lengths[i], m)) != 0) {
			cmatch = false;
		}

		/* check for error */
		if (!lmatch || !cmatch) {
			fprintf(stderr, "Problem in line %d: length match ? %d "
				"content match ? %d Content: \"%s\"\n",
				i, (int)lmatch, (int)cmatch, lp);
			success = false;
			break;
		}

		/* next */
		p += m;
		++i;
	} while (m != -1);

	/* Close */
	if (!feof(fp) || ferror(fp) || errno != 0) {
		fprintf(stderr, "stream state / errno wrong at end\n");
		success = false;
	}
	fclose(fp);

	/* Report */
	puts(success ? "-> pass" : "-> FAIL");
	return success;
}

int main(int argc, char** argv)
{
	bool verbose = true;

	/* Run tests */
	create_temp_file();
	bool success = true;
	success = success && check_getdelim(verbose, '\n', false, lengths1);
	success = success && check_getdelim(verbose, '\n', true, lengths1);
	success = success && check_getdelim(verbose, '1', false, lengths2);
	success = success && check_getdelim(verbose, '1', true, lengths2);
	unlink("tmpfile.txt");

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
