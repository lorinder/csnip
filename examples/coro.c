#include <stdio.h>
#include <stdbool.h>

#define CSNIP_SHORT_NAMES
#include <csnip/coro.h>

void* get_primes(coro* C, void* arg)
{
	const int N = *(int*)arg;
	if (N < 2)
		return NULL;

	int i = 2;
	coro_yield(C, &i);

	for (i = 3; i < N; i += 2) {
		bool isprime = true;
		for (int e = 3; e*e <= i; e += 2) {
			if (i % e == 0) {
				/* divisor found */
				isprime = false;
				break;
			}
		}

		if (isprime) {
			/* Prime found, report it */
			coro_yield(C, &i);
		}
	}

	return NULL;
}

int main(void)
{
	int nfound = 0;
	int N = 1000;

	/* Iterate over primes < N and print them */
	coro* C = coro_new();
	coro_set_func(C, get_primes, &N);
	while (coro_next(C, NULL, NULL) != 1) {
		int* v = coro_get_value(C);
		printf("%6d", *v);
		if (++nfound % 13 == 0)
			putchar('\n');
	}
	putchar('\n');
	coro_free(C);

	return 0;
}
