#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/x.h>

typedef struct {
	const char* input_string;
	const char* input_delim;

	char** output;
} strtok_example;

typedef enum {
	IMPL_GEN,
	IMPL_CSNIP,
} strtok_impl;

strtok_example testcases[] = {
  { "This is an example string.",
    " ",
    (char*[]){ "This", "is", "an", "example", "string.", NULL }
  },
  { "   This  is an example string.    ",
    " ",
    (char*[]){ "This", "is", "an", "example", "string.", NULL }
  },
  { "This is an example string.",
    "i ",
    (char*[]){ "Th", "s", "s", "an", "example", "str", "ng.", NULL }
  },
  { "xxxx",
    "x$",
    (char*[]){ NULL }
  },

  /* Sentinel */
  { NULL },
};

bool check_example(strtok_example* ex, strtok_impl imp)
{
	char* saveptr = NULL;
	char* str = strdup(ex->input_string);
	const char* delim = ex->input_delim;

	printf("Checking:  string=\"%s\", delimiters=\"%s\", imp=%d\n",
		str, delim, imp);

	char* s = NULL;
	int i = 0;
	do {
		/* Find next token */
		char* si = (i == 0 ? str : NULL);
		if (imp == IMPL_GEN) {
			s = x_strtok_r(si, delim, &saveptr);
		} else if (imp == IMPL_CSNIP) {
			s = x_strtok_r_imp(si, delim, &saveptr);
		}

		/* Compare */
		const bool same = (s == NULL && ex->output[i] == NULL)
			|| (s != NULL && ex->output[i] != NULL
			   && strcmp(s, ex->output[i]) == 0);
		if (!same) {
			fprintf(stderr, "Error: Mismatch iteration %d: "
				"Got \"%s\", expected \"%s\"\n",
				i, s ? s : "NULL",
				ex->output[i] ? ex->output[i] : "NULL");
			puts("-> FAIL");
			free(str);
			return false;
		}

		/* Next */
		if (ex->output[i] == NULL)
			break;
		++i;
	} while(1);

	puts("-> pass");
	free(str);
	return 1;
}

int main(int argc, char** argv)
{
	bool success = true;
	for (int i = 0; testcases[i].input_string != NULL; ++i) {
		success = check_example(&testcases[i], IMPL_GEN)
			  && check_example(&testcases[i], IMPL_CSNIP)
			  && success;
	}

	return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
