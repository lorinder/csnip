#ifndef CSNIP_RNG_MT_H
#define CSNIP_RNG_MT_H

/** @file rng_mt.h
 *  @defgroup rng_mt	Mersenne Twister
 *  @{
 *
 *  Mersenne twister RNG implementation
 */

#include <stdint.h>

#include <csnip/rng.h>

#define CSNIP_RNG_MT_N		624

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t state[CSNIP_RNG_MT_N];
	uint32_t* next;
} csnip_rng_mt_state;

/** Initialize a generic RNG descriptor. */
csnip_rng csnip_rng_mt_makerng(csnip_rng_mt_state* state);

/** Seed the MT Random number generator. */
void csnip_rng_mt_seed(csnip_rng_mt_state* S,
			int nseed,
			const uint32_t* seed);

/** Produce the next output number. */
uint32_t csnip_rng_mt_getnum(csnip_rng_mt_state* S);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CSNIP_RNG_MT_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_RNG_MT_HAVE_SHORT_NAMES)
#define rng_mt_state		csnip_rng_mt_state
#define rng_mt_makerng		csnip_rng_mt_makerng
#define rng_mt_seed		csnip_rng_mt_seed
#define rng_mt_getnum		csnip_rng_mt_getnum
#define CSNIP_RNG_MT_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_RNG_MT_HAVE_SHORT_NAMES */
