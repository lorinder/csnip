#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/heap.h>
#include <csnip/mem.h>
#include <csnip/sort.h>
#include <csnip/util.h>

/* Helper functions */

static int simple_rng(uint32_t* pseed, int lim)
{
	*pseed = 1664525*(*pseed) + 1013904223;
	return (int)((*pseed) / (UINT32_MAX + 1.0) * lim);
}

static int* make_rand_arr(int n, int rlim, uint32_t* pseed)
{
	int* a;
	mem_Alloc(n, a, _);
	for (int i = 0; i < n; ++i)
		a[i] = simple_rng(pseed, rlim);
	return a;
}

/* Heap methods */

CSNIP_HEAP_DEF_FUNCS(
	static cext_unused,			// scope
	IntHeap_,				// prefix
	args(int* a, int n, int k),		// args
	u, v,					// dummy vars
	a[u] < a[v],				// comparator
	Tswap(int, a[u], a[v]),			// swap
	k,					// arity
	n)					// array size


/* Test:
   1. Create random array,
      a) heapify and verify it's indeed a heap.
      b) repeatedly extract root:  should be in order.
 */
static bool check_heapsort(int n, int k, int rlim, uint32_t seed)
{
	printf("Test 1 (heapsort). size n = %d, arity k = %d, "
		"rng limit = %d\n", n, k, rlim);

	/* Create a heap */
	bool success = false;
	int* a = make_rand_arr(n, rlim, &seed);
	IntHeap_heapify(a, n, k);
	if (!IntHeap_check(a, n, k)) {
		puts("-> heap check after heapify() FAILED");
		goto done;
	}

	/* Repeatedly extract */
	for (int i = n - 1; i > 0; --i) {
		Tswap(int, a[0], a[i]);
		IntHeap_sift_down(a, i, k, 0);
	}

	/* Check if it's sorted */
	bool sorted;
	IsSorted(u, v, a[u] > a[v], n, sorted);
	if (!sorted) {
		puts("-> Unordered sequence after repeated extraction. "
		     "FAILED");
		goto done;
	}

	/* All completed, no failures */
	success = true;
done:
	mem_Free(a);
	return success;
}

/* Test:
   2. Make a heap by subsequent siftups and check it's a heap.
 */
static bool check_siftup(int n, int k, int rlim, uint32_t seed)
{
	printf("Test 2 (siftup make heap). size n = %d, arity k = %d, "
		"rng limit = %d\n", n, k, rlim);

	/* Create random array */
	int* a = make_rand_arr(n, rlim, &seed);

	/* Perform siftups to convert to heap */
	for (int i = 0; i < n; ++i) {
		IntHeap_sift_up(a, n, k, i);
	}

	/* Check */
	bool success = false;
	if (!IntHeap_check(a, n, k)) {
		puts("-> heap generation with siftup FAILED");
		goto done;
	}

	success = true;
done:
	mem_Free(a);
	return success;
}

/* Test:
   3. Make a heap, swap two unequal elements with any
      predecessor and verify that heap_Check() returns false.
 */
static bool check_heapcheck(int n, int k, int rlim, uint32_t seed)
{
	printf("Test 3 (heap check). size n = %d, arity k = %d, "
		"rng limit = %d\n", n, k, rlim);

	/* Skip non-applicable cases */
	if (n < 2)
		return true;

	/* Create a random heap */
	int* a = make_rand_arr(n, rlim, &seed);
	IntHeap_heapify(a, n, k);

	/* Modify to make it a non-heap
	 *
	 * We choose a random non-root element and its parent and swap
	 * them.  In case their value is the same, increase the value of
	 * the non-root before the swap.
	 *
	 * In this way, it will not ever be a heap.
	 */
	const int u = simple_rng(&seed, n - 1) + 1;
	const int v = (u - 1) / k;
	if (a[u] == a[v])
		++a[u];
	Tswap(int, a[u], a[v]);

	/* Check */
	bool success = false;
	if (IntHeap_check(a, n, k)) {
		puts("-> heap check FAILED to reject a non-heap");
		goto done;
	}

	success = true;
done:
	mem_Free(a);
	return success;
}

/* Test:
   4. Decrease(a)/Increase(b)/Change(c) the value of a random element in
      a heap, sift it up(a)/down(b)/(c), and check that it's a heap.
 */

static void mod_upprio(int* a, int n, int k, int rlim, int u, int delta)
{
	a[u] -= delta;
	IntHeap_sift_up(a, n, k, u);
}

static void mod_downprio(int* a, int n, int k, int rlim, int u, int delta)
{
	a[u] += delta;
	IntHeap_sift_down(a, n, k, u);
}

typedef void (*ModFunc)(int* a, int n, int k, int rlim, int u, int delta);

static bool check_sift(int n, int k, int rlim, uint32_t seed,
	char subtype, ModFunc mod_func)
{
	printf("Test 4%c. size n = %d, arity k = %d, "
		"rng limit = %d\n", subtype, n, k, rlim);

	/* Skip non-applicable cases */
	if (n == 0)
		return true;

	/* Create random heap */
	int* a = make_rand_arr(n, rlim, &seed);
	IntHeap_heapify(a, n, k);

	/* Change a couple of elements */
	const int m = 10;
	for (int i = 0; i < m; ++i) {
		const int u = simple_rng(&seed, n);
		const int delta = simple_rng(&seed, n);
		(*mod_func)(a, n, k, rlim, u, delta);
	}

	/* Check */
	bool success = false;
	if (!IntHeap_check(a, n, k)) {
		puts("-> heap check FAILED after changes");
		goto done;
	}

	success = true;
done:
	mem_Free(a);
	return success;
}

int main(int argc, char** argv)
{
	const int ns[] = { 0, 1, 2, 3, 4, 17, 123, 128, 997, 1024,
			   65531, 65535, 65536 };
	const int ks[] = { 2, 3, 4, 5, 6, 7, 8 };
	const int rlims[] = { 1, 100, 10000, 1000000, 100000000 };
	unsigned seed = 1;
	for (int ni = 0; ni < Static_len(ns); ++ni) {
		const int n = ns[ni];
		for (int ki = 0; ki < Static_len(ks); ++ki) {
			const int k = ks[ki];
			for (int rlimi = 0; rlimi < Static_len(rlims); ++rlimi)
			{
				const int rlim = rlims[rlimi];

				/* Run tests */
				if (!check_heapsort(n, k, rlim, seed++)
				  || !check_siftup(n, k, rlim, seed++)
				  || !check_heapcheck(n, k, rlim, seed++)
				  || !check_sift(n, k, rlim, seed++, 'a', mod_upprio)
				  || !check_sift(n, k, rlim, seed++, 'b', mod_downprio)
				  ) // || !check_chgprio(n, k, rlim, seed++))
				{
					fprintf(stderr, "==> FAILURE\n");
					return 1;
				}
			}
		}
	}

	return 0;
}
