#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/find.h>
#include <csnip/util.h>

int main()
{
	int a[] = {0, 1, 2, 3, 4};
	const int N = Static_len(a);

	/* Search with indices */
	for (int i = -1; i < N + 2; ++i) {
		int ret;
		Bsearch(int, u, a[u] < i, N, ret);
		printf("search for %d: index %d\n", i, (int)ret);
	}

	/* Search for intermediate values */
	for (int i = -1; i < N + 1; ++i) {
		const float target = i + 0.5;
		int ret;
		Bsearch(int, u, a[u] < target, N, ret);
		printf("search for %g: index %d with value %d\n", target, ret,
			ret < N ? a[ret] : -999);
	}
	return 0;
}
