#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define CSNIP_SHORT_NAMES
#include <csnip/util.h>

/* Unsophisticated tests of the utility functions. */

#define CHECK(x) \
	do { \
		if (!(x)) { \
			fprintf(stderr, \
			  "Assertion `%s' does not hold\n", \
			  #x); \
			exit(1); \
		} \
	} while(0)

void test_min(void)
{
	CHECK(Min(-10, 10) == -10);
	CHECK(Min(3, 2) == 2);
	CHECK(Min(-1.2, 2) == -1.2);
}

void test_max(void)
{
	CHECK(Max(-10, 10) == 10);
	CHECK(Max(3, 2) == 3);
	CHECK(Max(-1.2, 2) == 2);
}

void test_clamp(void)
{
	CHECK(Clamp(-3, -4, 3) == -3);
	CHECK(Clamp(-3, -2, 3) == -2);
	CHECK(Clamp(-3, 5, 3) == 3);
}

struct X {
	int u;
	double v;
};

void test_container_of(void)
{
	struct X c;
	CHECK(Container_of(&c.v, struct X, v) == &c);
	CHECK(Container_of(&c.u, struct X, u) == &c);
}

int main(void)
{
	test_min();
	test_max();
	test_clamp();
	test_container_of();
	return 0;
}
