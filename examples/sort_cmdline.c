#include <stdio.h>
#include <stdlib.h>

#include <csnip/util.h>
#include <csnip/arr.h>
#include <csnip/sort.h>

int* arr;
int arr_sz, arr_cap;

CSNIP_ARR_DEF_FUNCS(static inline, intarr_, int, noargs(),
			arr, arr_sz, arr_cap, _)

static void printarr()
{
	int i;
	for (i = 0; i < arr_sz; ++i) {
		printf("%d ", arr[i]);
	}
	putchar('\n');
}

int main(int argc, char** argv)
{
	intarr_init(8);

	/* Read integers from command line */
	int i;
	for (i = 1; i < argc; ++i) {
		int j = atoi(argv[i]);
		//printf(">> %d\n", j);
		intarr_push(j);
	}

	/* Output test set */
	printf("Before sorting: ");
	printarr();
	_Bool is_sorted;
	csnip_IsSorted(u, v, arr[u] < arr[v], arr_sz, is_sorted);
	printf("is_sorted? %d\n", is_sorted);

	/* Sort */
	csnip_Qsort(u, v, arr[u] < arr[v],
		csnip_Tswap(int, arr[u], arr[v]),
		arr_sz);

	/* Display result */
	printf("After sorting: ");
	printarr();
	csnip_IsSorted(u, v, arr[u] < arr[v], arr_sz, is_sorted);
	printf("is_sorted? %d\n", is_sorted);

	return 0;
}
