#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/util.h>
#include <csnip/meanvar.h>

int main(void)
{
	const double values[] = { 1, 13, 5, 6, 8, 2, 3.3 };

	meanvar X = { 0 };
	printf("values:");
	for (int i = 0; i < Static_len(values); ++i) {
		printf(" %g", values[i]);
		meanvar_add(&X, values[i]);
	}
	printf("\n-> %ld values, mean = %g, sample variance = %g\n",
		X.count,
		meanvar_mean(&X),
		meanvar_var(&X, 1));

	return 0;
}
