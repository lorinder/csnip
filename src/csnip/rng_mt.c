#define HAVE_STDINT_H
#define HAVE_STDLIB_H
#define HAVE_UNISTD_H
#define HAVE_TIME_H

#define CSNIP_SHORT_NAMES
#include <csnip/rng.h>
#include <csnip/rng_mt.h>
#include <csnip/mem.h>

static void gen_seed(const csnip_rng* R,
					int nseed,
					const unsigned long int* seed)
{
	// XXX: We're assuming here that unsigned long int pointers
	// can be punned to uint32_t pointers.  Rethink this.
	rng_mt_seed(R->state,
			nseed * sizeof(unsigned long int) / sizeof(uint32_t),
			(uint32_t*)seed);
}

static unsigned long int gen_getnum(const csnip_rng* R)
{
	return rng_mt_getnum(R->state);
}

rng csnip_rng_mt_makerng(csnip_rng_mt_state* state)
{
	rng R = {
		.minval = 0,
		.maxval = 0xFFFFFFFFul,
		.state = state,
		.seed = gen_seed,
		.getnum = gen_getnum,
	};

	return R;
}



void csnip_rng_mt_seed(csnip_rng_mt_state* S,
			int nseed,
			const unsigned int* seed)
{
	S->state[0] = seed[0];
	uint32_t prev = S->state[0];
	for (int i = 1; i < CSNIP_RNG_MT_N; ++i) {
		S->state[i] = 1812433253 * (prev ^ (prev >> 30)) + i;
		prev = S->state[i];
	}
	S->next = S->state + CSNIP_RNG_MT_N;
}

#define MT_M		397

static uint32_t mixbits(uint32_t u, uint32_t v)
{
	return (u & 0x80000000) | (v & 0x7fffffff);
}

static uint32_t twist(uint32_t u, uint32_t v)
{
	const uint32_t matrix_A = 0x9908b0df;
	return (mixbits(u, v) >> 1) ^ (v & 1 ? matrix_A : 0);
}

static void
update_state(csnip_rng_mt_state* S)
{
	uint32_t* p = S->state;
	for (int count = CSNIP_RNG_MT_N - MT_M + 1; --count; ++p)
		*p = p[MT_M] ^ twist(p[0], p[1]);
	for (int count = MT_M; --count; ++p)
		*p = p[MT_M - CSNIP_RNG_MT_N] ^ twist(p[0], p[1]);
	*p = p[MT_M - CSNIP_RNG_MT_N] ^ twist(p[0], S->state[0]);
	S->next = S->state;
}

uint32_t csnip_rng_mt_getnum(csnip_rng_mt_state* S)
{
	if (S->next == S->state + CSNIP_RNG_MT_N)
		update_state(S);
	uint32_t r = *S->next++;

	/* Tempering */
	r ^= r >> 11;
	r ^= (r << 7) & 0x9d2c5680;
	r ^= (r << 15) & 0xefc60000;
	r ^= r >> 18;

	return r;
}
