#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <csnip/coro.h>

char* days[] = {
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday",

	/* Sentinel */
	NULL
};

static void* enumerate_days(csnip_coro* C, void* arg)
{
	(void)arg;
	for (int i = 0; days[i]; ++i) {
		csnip_coro_yield(C, days[i]);
	}
	return NULL;
}

/* Iterate, detect end based on returned pointer */
static bool test1(csnip_coro* C)
{
	puts("Test 1: Enumeration");
	int i = 0;
	csnip_coro_set_func(C, enumerate_days, C);
	do {
		void* p;
		csnip_coro_next(C, NULL, &p);
		if (p == NULL)
			break;

		/* Check what we got */
		if (i >= 7 || strcmp(days[i], p) != 0)
			return false;
		puts(p);
		++i;
	} while(1);

	puts("-> passed\n");
	return true;
}

/* Iterate, detect end based on status info */
static bool test2(csnip_coro* C)
{
	puts("Test 2: Enumeration, variant 2");
	int i = 0;
	csnip_coro_set_func(C, enumerate_days, C);
	do {
		void* p;
		if (csnip_coro_next(C, NULL, &p) != 0)
			break;

		/* Check output */
		if (i >= 7 || strcmp(days[i], p) != 0)
			return false;
		puts(p);
		++i;
	} while(1);

	puts("-> passed\n");
	return true;
}


int main(int argc, char** argv)
{
	/* Simple enumeration tests */
	csnip_coro* C = csnip_coro_new();
	if (!test1(C))
		return EXIT_FAILURE;
	if (!test2(C))
		return EXIT_FAILURE;
	csnip_coro_free(C);

	/* Clean up */
	return EXIT_SUCCESS;
}
