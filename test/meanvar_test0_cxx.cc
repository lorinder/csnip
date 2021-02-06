/* Smoke test for CXX meanvar */

#include <cstdio>
using std::printf;

#define CSNIP_SHORT_NAMES
#include <csnip/meanvar.h>
#include <csnip/util.h>

int main(void)
{
	float f[] = {1, 2, 3, 4};
	meanvar X = { 0 };
	for (int i = 0; i < (int)Static_len(f); ++i) {
		meanvar_Add(&X, f[i]);
	}
	printf("-> got %ld elements, mean=%g, var=%g\n",
		X.count,
		meanvar_Mean(&X),
		meanvar_Var(&X, 1));
	return 0;
}
