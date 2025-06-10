/* Smoke test for CXX meanvar */

#include <cstdio>
using std::printf;

#define CSNIP_SHORT_NAMES
#include <csnip/meanvar.h>
#include <csnip/util.h>

int main(void)
{
	/* X set */
	float f[] = {1, 2, 3, 4};
	meanvar X = { 0 };
	for (int i = 0; i < (int)Static_len(f); ++i) {
		meanvar_Add(&X, f[i]);
	}
	printf("X: got %ld elements, mean=%g, var=%g\n",
		X.count,
		meanvar_Mean(&X),
		meanvar_Var(&X, 1));

	/* Y set */
	float g[] = {5, 6, 7, 8};
	meanvar Y = { 0 };
	for (int i = 0; i < (int)Static_len(g); ++i) {
		meanvar_Add(&Y, g[i]);
	}
	printf("Y: got %ld elements, mean=%g, var=%g\n",
		Y.count,
		meanvar_Mean(&Y),
		meanvar_Var(&Y, 1));

	/* Merge the two */
	meanvar_Merge(&X, &Y);
	printf("X + Y: got %ld elements, mean=%g, var=%g\n",
		X.count,
		meanvar_Mean(&X),
		meanvar_Var(&X, 1));
	return 0;
}
