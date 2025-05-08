#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <csnip/cext.h>
#include <csnip/mem.h>
#include <csnip/mempool.h>

typedef struct {
	uint64_t v;
} My;

CSNIP_MEMPOOL_DEF_TYPE(MyPool_s, My)
typedef struct MyPool_s MyPool;

CSNIP_MEMPOOL_DECL_FUNCS(static csnip_cext_unused,
		MyPool_,
		My,
		MyPool)
CSNIP_MEMPOOL_DEF_FUNCS(static csnip_cext_unused,
		MyPool_,
		My,
		MyPool)

typedef struct {
	uint64_t v;
	My* item;
} MyTracker;

int main(void)
{
	const int N = 1000;
	MyTracker* t;
	csnip_mem_Alloc(N, t, _);
	for (int i = 0; i < N; ++i) {
		t[i] = (MyTracker) { 0 };
	}

	MyPool pool = MyPool_init_empty();

	const int M = 10000;
	for (int i = 0; i < M; ++i) {
		double r = rand() / (RAND_MAX + 1.0);
		const int j = (int)(r * N);
		if (t[j].item != NULL) {
#if 1
			MyPool_free_item(&pool, t[j].item);
#else
			free(t[j].item);
#endif
			t[j].item = NULL;
		} else {
#if 1
			t[j].item = MyPool_alloc_item(&pool, NULL);
#else
			t[j].item = malloc(sizeof(My));
#endif
			t[j].v = i;
			t[j].item->v = i;
		}
	}

	/* Check */
	int pass = 1;
	int nit = 0;
	for (int i = 0; i < N; ++i) {
		if (t[i].item) {
			++nit;
			if (t[i].v != t[i].item->v) {
				fprintf(stderr, "Error:  Value mismatch at %d: %"
						PRIu64" vs %"PRIu64"\n", i, t[i].v,
						t[i].item->v);
				pass = 0;
			}
		}
	}
	printf("Number of used items found: %d\n", nit);

	puts(pass ? "pass" : "FAIL");
	return (pass ? EXIT_SUCCESS : EXIT_FAILURE);
}
