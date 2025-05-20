#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <csnip/arr.h>
#include <csnip/cext.h>

typedef struct {
	int* a;
	int n;
	int cap;
} IntArr;

CSNIP_ARR_DECL_FUNCS(
	static,				// scope
	IntArr_,			// prefix
	int,				// value type
	args(IntArr*, int*)		// gen_args
)

CSNIP_ARR_DEF_FUNCS(
	static csnip_cext_unused,	// static
	IntArr_,			// scope
	int,				// value type
	args(IntArr* A, int* err),	// gen_args
	A->a,				// array
	A->n,				// array size
	A->cap,				// array capacity
	*err				// error return
)

static bool test_reserve()
{
	IntArr A;
	IntArr_init(&A, NULL, 0);

	int err = 0;
	IntArr_reserve(&A, &err, 10);
	if (err != 0 || A.cap < 10)
		return false;

	/* Make array to be of size 5 */
	A.n = 5;

	IntArr_reserve(&A, &err, 100);
	if (err != 0 || A.cap < 100)
		return false;

	IntArr_reserve(&A, &err, 12);
	if (err != 0 || A.cap < 12)
		return false;

	IntArr_reserve(&A, &err, 5);
	if (err != 0 || A.cap < 5)
		return false;

	IntArr_reserve(&A, &err, 4);
	err = 0;	// XXX currently this doesn't fail, but it could
			// make sense to make it fail.
	if (A.cap < 5)
		return false;

	IntArr_deinit(&A, &err);
	return true;
}

static bool test_push()
{
	IntArr A;
	IntArr_init(&A, NULL, 0);

	for (int i = 0; i < 1000; ++i)
		IntArr_push(&A, NULL, i);

	/* Check */
	if (A.n != 1000) {
		return false;
	}
	for (int i = 0; i < 1000; ++i) {
		if (A.a[i] != i)
			return false;
	}

	IntArr_deinit(&A, NULL);
	return true;
}

static bool test_pop()
{
	IntArr A;
	IntArr_init(&A, NULL, 0);
	for (int i = 0; i < 1000; ++i)
		IntArr_push(&A, NULL, i);


	for (int i = 0; i < 1000; ++i)
		IntArr_pop(&A, NULL);
	if (A.n != 0) {
		fputs("Didn't get 0 size after popping everything.\n",
			stderr);
		return false;
	}
	int err = 0;
	IntArr_pop(&A, &err);
	if (err != csnip_err_UNDERFLOW) {
		fputs("arr_Pop() empty array didn't result in expected"
			"underflow error.\n", stderr);
		return false;
	}

	IntArr_deinit(&A, NULL);
	return true;
}

static bool test_insert_at()
{
	const int N = 700;
	IntArr A;
	IntArr_init(&A, NULL, 0);
	for (int i = 0; i < N; ++i) {
		if (i & 1) {
			IntArr_insert_at(&A, NULL, 0, i);
		} else {
			IntArr_insert_at(&A, NULL, i/2, i);
		}
	}

	/* Check it */
	for (int i = 0; i < N/2; ++i) {
		int expected = N - 2*i - 1;
		if (A.a[i] != expected) {
			fprintf(stderr, "Array entry %d not what was "
			  "expected. Is %d, expected %d.\n",
			  i, A.a[i], expected);
			return false;
		}

		const int j = i + N/2;
		expected = N - 2*i - 2;
		if (A.a[j] != expected) {
			fprintf(stderr, "Array entry %d not what was "
			  "expected. Is %d, expected %d.\n",
			  j, A.a[j], expected);
			return false;
		}
	}

	IntArr_deinit(&A, NULL);
	return true;
}

static bool test_delete_at()
{
	const int N = 230;
	IntArr A;
	IntArr_init(&A, NULL, 0);
	for (int i = 0; i < N; ++i) {
		IntArr_push(&A, NULL, i);
	}

	for (int i = 0; i < N/2; ++i) {
		IntArr_delete_at(&A, NULL, i + 1);
	}

	if (A.n != N/2) {
		fputs("Not the expected size after deleting "
			"half the elements.\n", stderr);
		return false;
	}
	for (int i = 0; i < N/2; ++i) {
		if (A.a[i] != 2*i) {
			fprintf(stderr, "Entry %d has value %d, expected %d\n",
				i, A.a[i], 2*i);
			return false;
		}
	}

	IntArr_deinit(&A, NULL);
	return true;
}

static bool test_deinit()
{
	IntArr A;
	IntArr_init(&A, NULL, 256);

	IntArr_deinit(&A, NULL);
	if (A.cap != 0 || A.a != NULL)
		return false;

	IntArr_deinit(&A, NULL); // deinit a 2nd time is a no-op.
	return true;
}

int main(int argc, char** argv)
{
	bool success = true;

	printf("Overall result: %s\n", (success ? "pass" : "FAIL"));
#define TEST(x) do { \
		printf("testing \"%s\"\n", #x); \
		const bool pass = test_ ## x(); \
		if (!pass) \
			puts("-> FAIL"); \
		else \
			puts("-> pass");  \
		success = pass && success; \
	} while (0)
	TEST(reserve);
	TEST(push);
	TEST(pop);
	TEST(insert_at);
	TEST(delete_at);
	TEST(deinit);
#undef TEST

	printf("Overall result: %s\n", (success ? "pass" : "FAIL"));
	return (success == true ? 0 : 1);
}
