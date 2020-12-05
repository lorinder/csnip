#include <stdio.h>
#include <stdbool.h>

#include <csnip/coro.h>

typedef struct {
	int pc;
	int i;
} get_primes_state;

int get_primes(get_primes_state* S, int N)
{
	CSNIP_CORO_START(S->pc)
#	define i (S->i)

	if (N < 2)
		return -1;

	i = 2;
	csnip_coro_Yield(S->pc, __LINE__, i);

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
			csnip_coro_Yield(S->pc, __LINE__, i);
		}
	}

#	undef i
	CSNIP_CORO_END(S->pc)
	return -1;
}

int main(void)
{
	int nfound = 0;
	int N = 1000;

	/* Iterate over primes < N and print them */
	int p;
	get_primes_state S = { 0 };
	while ((p = get_primes(&S, N)) != -1) {
		printf("%6d", p);
		if (++nfound % 13 == 0)
			putchar('\n');
	}
	putchar('\n');

	return 0;
}
