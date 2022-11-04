#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/list.h>
#include <csnip/mem.h>
#include <csnip/util.h>

typedef struct Ent_s {
	int val;
	struct Ent_s *ptr_prev, *ptr_next;
} Ent;

typedef struct {
	Ent *head, *tail;
} EntList;

CSNIP_DLIST_DECL_FUNCS(static cext_unused, EntList_, Ent*, args(EntList*))
CSNIP_DLIST_DEF_FUNCS(static, EntList_, Ent*, args(EntList* list),
	list->head, list->tail, ptr_prev, ptr_next);

void free_all_members(EntList* L)
{
	while (L->head != NULL) {
		Ent* e = L->head;
		EntList_pop_head(L);
		mem_Free(e);
	}
}

/* push_tail test */
bool test_push_tail(int n)
{
	EntList L;
	EntList_init(&L);

	/* Fill one after another */
	for (int i = 0; i < n; ++i) {
		Ent* e;
		mem_Alloc(1, e, _);
		*e = (Ent){ .val = i };
		EntList_push_tail(&L, e);
	}

	/* Check expected outcome */
	int i = 0;
	Ent* e = L.head;
	for (; i < n; ++i) {
		if (e == NULL || e->val != i)
			return false;
		e = e->ptr_next;
	}
	if (e != NULL || i != n)
		return false;

	free_all_members(&L);
	return true;
}

static bool test_insert_before0(int n, const int* v)
{
	EntList L;
	EntList_init(&L);

	/* Insert the elements in ascending order */
	for (int i = 0; i < n; ++i) {
		const int u = v[i];

		/* Create element to insert */
		Ent* newEl;
		mem_Alloc(1, newEl, _);
		*newEl = (Ent){ .val = u };

		/* Find insertion spot & insert*/
		Ent* e = L.head;
		while (e != NULL && e->val < u)
			e = e->ptr_next;
		EntList_insert_before(&L, e, newEl);
	}

	/* Debug out */
	if (0) {
		printf("List:");
		Ent* e = L.head;
		while (e != NULL) {
			printf(" %d", e->val);
			e = e->ptr_next;
		}
		putchar('\n');
	}

	/* Check */
	int i = 0;
	Ent* e = L.head;
	while (i < n && e != NULL) {
		if (e->val != i) {
			fprintf(stderr, "error at %d -> %d\n", i, e->val);
			return false;
		}
		++i;
		e = e->ptr_next;
	}
	if (i != n || e != NULL) {
		fprintf(stderr, "err at end: i == %d, e == %p\n", i, e);
		return false;
	}

	free_all_members(&L);
	return true;
}

bool test_insert_before(void)
{
	EntList L;
	EntList_init(&L);

	/* A few test cases */
#define TEST_CASE(...) do { \
		const int v[] = { __VA_ARGS__ }; \
		if (!test_insert_before0(Static_len(v), v)) \
			return false; \
	} while(0)
	TEST_CASE(0, 5, 6, 9, 4, 8, 3, 1, 2, 7);
	TEST_CASE(1, 0, 4, 9, 5, 2, 3, 7, 8, 6);
	TEST_CASE(9, 1, 0, 2, 4, 5, 6, 3, 7, 8);
	TEST_CASE(2, 6, 3, 9, 1, 7, 0, 5, 4, 8);
	TEST_CASE(4, 9, 5, 2, 0, 7, 6, 8, 3, 1);
	TEST_CASE(1, 5, 9, 7, 0, 8, 6, 2, 3, 4);
	TEST_CASE(3, 2, 8, 0, 7, 5, 1, 4, 6, 9);
	TEST_CASE(8, 4, 3, 5, 1, 2, 6, 9, 0, 7);
	TEST_CASE(1, 6, 8, 0, 7, 5, 3, 2, 9, 4);
	TEST_CASE(5, 2, 3, 9, 8, 1, 4, 0, 7, 6);
	TEST_CASE(0, 2, 9, 1, 8, 6, 7, 3, 4, 5);
	TEST_CASE(7, 3, 1, 5, 2, 8, 4, 0, 9, 6);
#undef TEST_CASE

	return true;
}

// XXX Copy paste from test_insert_before
static bool test_insert_after0(int n, const int* v)
{
	EntList L;
	EntList_init(&L);

	/* Insert the elements in ascending order */
	for (int i = 0; i < n; ++i) {
		const int u = v[i];

		/* Create element to insert */
		Ent* newEl;
		mem_Alloc(1, newEl, _);
		*newEl = (Ent){ .val = u };

		/* Find insertion spot & insert*/
		Ent* e = L.tail;
		while (e != NULL && u < e->val)
			e = e->ptr_prev;
		EntList_insert_after(&L, e, newEl);
	}

	/* Check */
	int i = 0;
	Ent* e = L.head;
	for (; i < n && e != NULL; ++i, e = e->ptr_next) {
		if (e->val != i)
			return false;
	}
	if (i != n || e != NULL) {
		return false;
	}

	free_all_members(&L);
	return true;
}

// XXX: Quasi-copy-paste from test_insert_before
bool test_insert_after(void)
{
	EntList L;
	EntList_init(&L);

	/* A few test cases */
#define TEST_CASE(...) do { \
		const int v[] = { __VA_ARGS__ }; \
		if (!test_insert_after0(Static_len(v), v)) \
			return false; \
	} while(0)
	TEST_CASE(0, 5, 6, 9, 4, 8, 3, 1, 2, 7);
	TEST_CASE(1, 0, 4, 9, 5, 2, 3, 7, 8, 6);
	TEST_CASE(9, 1, 0, 2, 4, 5, 6, 3, 7, 8);
	TEST_CASE(2, 6, 3, 9, 1, 7, 0, 5, 4, 8);
	TEST_CASE(4, 9, 5, 2, 0, 7, 6, 8, 3, 1);
	TEST_CASE(1, 5, 9, 7, 0, 8, 6, 2, 3, 4);
	TEST_CASE(3, 2, 8, 0, 7, 5, 1, 4, 6, 9);
	TEST_CASE(8, 4, 3, 5, 1, 2, 6, 9, 0, 7);
	TEST_CASE(1, 6, 8, 0, 7, 5, 3, 2, 9, 4);
	TEST_CASE(5, 2, 3, 9, 8, 1, 4, 0, 7, 6);
	TEST_CASE(0, 2, 9, 1, 8, 6, 7, 3, 4, 5);
	TEST_CASE(7, 3, 1, 5, 2, 8, 4, 0, 9, 6);
#undef TEST_CASE

	return true;
}

int main()
{
	bool success = true;
#define RUN_TEST(x) do { \
		printf("Running \"%s\" ... ", #x); \
		fflush(stdout); \
		if (!(x)) { \
			puts("FAILED"); \
			success = false; \
		} else { \
			puts("success"); \
		} \
	} while (0);
	RUN_TEST(test_push_tail(10));
	RUN_TEST(test_insert_before());
	RUN_TEST(test_insert_after());
#undef RUN_TEST

	return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
