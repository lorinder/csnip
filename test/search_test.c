/* Tests for the Bsearch and Lsearch macros */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>
#include <csnip/search.h>
#include <csnip/sort.h>
#include <csnip/util.h>

static uint32_t rnext(uint64_t* pstate)
{
	*pstate *= UINT64_C(6364136223846793005);
	*pstate += 1;

	return (uint32_t)(*pstate >> 32);
}

static uint32_t reduce_to_bits(int n_bits, uint32_t v)
{
	if (n_bits == 0)
		return 0;
	return v >> (32 - n_bits);
}

int main(int argc, char** argv)
{
	uint64_t rstate = 1234;
	const int Ns[] = { 0, 1, 2, 3, 10, 100, 1000 };
	const int bs[] = { 0, 1, 2, 3, 4, 8, 16, 32 };
	int k = 3;

	for (int Ni = 0; Ni < Static_len(Ns); ++Ni) {
		const int N = Ns[Ni];
		printf("N = %d\n", N);

		/* Create the instance */
		uint32_t* a;
		mem_Alloc(N, a, _);
		for (int i = 0; i < N; ++i) {
			a[i] = rnext(&rstate);
		}
		Qsort(u, v, a[u] < a[v], Tswap(uint32_t, a[u], a[v]), N);

		/* Check with different bit widths.
		 *
		 * The point here is that down projecting to a fixed
		 * number of bits causes progressively more comparisons
		 * to the same values, which changes the nature of the
		 * search.
		 */
		for (int bi = 0; bi < Static_len(bs); ++bi) {
			const int b = bs[bi];
			printf("  b = %d\n", b);
			for (int i = 0; i < k; ++i) {
				int idx;
#define red(x) reduce_to_bits(b, (x))
				uint32_t d = red(rnext(&rstate));

				/* Variants
				 * 1a) smallest i s.t. a[i] >= key
				 * 1b) largest i s.t. a[i] < key (satisfied by i - 1)
				 */
				Bsearch(int, u, red(a[u]) < d, N, idx);
				printf("    d = %"PRIu32" -> V1_idx = %d ", d, idx);
				if (idx < 0 || idx > N) {
					fprintf(stderr, "Error:  Result out "
					  "of range (N = %d): %d\n", N, idx);
					return 1;
				}
				if (idx < N && red(a[idx]) < d) {
					fprintf(stderr, "Error: V1 constraint "
					  "not met.\n");
					return 1;
				}
				if (idx > 0 && red(a[idx - 1]) >= d) {
					fprintf(stderr, "Error: V1 minimality "
					  "not satisfied.\n");
					return 1;
				}

				/* Variants
				 * 2a) smallest i s.t. a[i] > key
				 * 2b) largest i s.t.  a[i] <= key (satisfied by i - 1)
				 */
				Bsearch(int, u, red(a[u]) <= d, N, idx);
				printf("V2_idx = %d ", idx);
				if (idx < 0 || idx > N) {
					fprintf(stderr, "Error:  Result out "
					  "of range (N = %d): %d\n", N, idx);
					return 1;
				}
				if (idx < N && red(a[idx]) <= d) {
					fprintf(stderr, "Error: V2 constraint "
					  "not met.\n");
					return 1;
				}
				if (idx > 0 && red(a[idx - 1]) > d) {
					fprintf(stderr, "Error: V2 minimality "
					  "not satisfied.\n");
					return 1;
				}
#undef red

				putchar('\n');
			}

			printf("    All checks pass.\n");
		}
	}
	return 0;
}
