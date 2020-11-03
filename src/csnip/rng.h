#ifndef CSNIP_RNG_H
#define CSNIP_RNG_H

/**	@file rng.h
 *	@defgroup rng		Random Number Generation
 *	@{
 *
 *	Abstract base type for random number generation.
 */

/**	Method table for a random number generator.  */
struct csnip_rng_T {
	unsigned long minval;		/**< Minimum value returned by RNG, inclusive. */
	unsigned long maxval;		/**< Maximum value returned by RNG, inclusive. */

	void* state;			/**< The random number generator state.  Size
					     and content dependent on the RNG in use. */

	/** Seed the RNG with the given seed memory. */
	void (*seed)(const struct csnip_rng_T* rng,
		     int nseed,
	             const unsigned long* seed);
	/** Retrieve the next random number and update state. */
	unsigned long int (*getnum)(
			const struct csnip_rng_T* rng);
};

typedef struct csnip_rng_T csnip_rng;

/** Retrieve a number from the RNG.
 *
 *  Shorthand for R->getnum(R).
 */
inline unsigned long int csnip_rng_getnum(const csnip_rng* R)
{
	return (*R->getnum)(R);
}

/** Seed the RNG.
 *
 *  Given seeding memory, seed the RNG.
 */
inline void csnip_rng_seed(const csnip_rng* R,
				int nseed,
				const unsigned long* seed)
{
	(*R->seed)(R, nseed, seed);
}

#ifdef CSNIP_SHORT_NAMES
#define	rng		csnip_rng
#define rng_getnum	csnip_rng_getnum
#define rng_seed	csnip_rng_seed
#endif

/** @} */

#endif /* CSNIP_RNG_H */
