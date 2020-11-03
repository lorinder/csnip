#include <csnip/rng.h>

extern inline unsigned long int csnip_rng_getnum(const csnip_rng* R);
extern inline void csnip_rng_seed(const csnip_rng* R,
				int nseed,
				const unsigned long* seed);
