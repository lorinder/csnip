#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <csnip/clopts.h>
#include <csnip/meanvar.h>
#include <csnip/util.h>

typedef struct {
	float mean;
	float var0;
	float var1;
} resultf;

const float v1[] = { 1, 2, 3, 4, 3.2f };
const resultf exp1 = { .mean = 2.64f, .var0 = 1.0784f, .var1 = 1.348f };

static _Bool check_got_vs_exp(const resultf* got,
				const resultf* exp,
				float epsilon)
{
	_Bool success = 1;
	if (fabsf(exp->mean - got->mean) > epsilon) {
		fprintf(stderr, "  Mismatch in mean, got %g, expected %g\n",
			got->mean, exp->mean);
		success = 0;
	}
	if (fabsf(exp->var0 - got->var0) > epsilon) {
		fprintf(stderr, "  Mismatch in var0, got %g, expected %g\n",
			got->var0, exp->var0);
		success = 0;
	}
	if (fabsf(exp->var1 - got->var1) > epsilon) {
		fprintf(stderr, "  Mismatch in var1, got %g, expected %g\n",
			got->var1, exp->var1);
		success = 0;
	}
	return success;
}

static _Bool check_testcase(const float* f, int N,
				const resultf* exp,
				float epsilon)
{
	/* Describe instance */
	printf("Looking at instance: {");
	const int maxdisp = 8;
	for (int i = 0; i < maxdisp && i < N; ++i) {
		printf(" %g", f[i]);
	}
	if (N > maxdisp) {
		printf(" ... (%d values total)", N);
	}
	printf(" }\n");
	printf("  expecting:  mean %g  var0 %g  var1 %g\n",
		exp->mean, exp->var0, exp->var1);

	/* Compute */
	csnip_meanvarf A = { 0 };
	for (int i = 0; i < N; ++i)
		csnip_meanvar_Add(&A, f[i]);
	const resultf got = {
		.mean = csnip_meanvar_Mean(&A),
		.var0 = csnip_meanvar_Var(&A, 0),
		.var1 = csnip_meanvar_Var(&A, 1)
	};
	printf("  got:        mean %g  var0 %g  var1 %g\n",
		got.mean, got.var0, got.var1);
	_Bool result = check_got_vs_exp(&got, exp, epsilon);
	if (result) {
		printf("-> success\n");
	} else {
		printf("-> FAIL\n");
	}
	return result;
}


int main(int argc, char** argv)
{
	/* Parse args */
	csnip_clopts opts = {
	  .description = "Testing tool for the meanvar module",
	};
	csnip_clopts_add_defaults(&opts);
	if (csnip_clopts_process(&opts, argc - 1, argv + 1, NULL, true) < 0)
		return EXIT_FAILURE;

	check_testcase(v1, csnip_Static_len(v1), &exp1, 0.001f);

	return EXIT_SUCCESS;
}
