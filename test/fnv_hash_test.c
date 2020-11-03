#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CSNIP_SHORT_NAMES
#include <csnip/hash.h>

struct testvector {
	const char* str;
	unsigned long int h32;
	unsigned long long int h64;
};

struct testvector tvs[] = {
  { "", 0x811c9dc5, 0xcbf29ce484222325},
  { "Hello\n", 0x81218953, 0x922513e07c5a21d3},
  { "\xde\xad\xbe\xef", 0x045d4bb3, 0x277045760cdd0993},
  { "0", 0x350ca8af, 0xaf63ad4c86019caf},
  { "1", 0x340ca71c, 0xaf63ac4c86019afc},
  { "2", 0x370cabd5, 0xaf63af4c8601a015},
  { "00", 0x21ed724d, 0x07fc1807b4bd222d},
  { "\033[01;32m", 0xcaef1bcb, 0xec300134fb9657ab},
  { "/etc/hosts", 0xfd999c48, 0xab0a49b3f5f1fd68},

  /* Sentinel */
  { NULL }
};

int main()
{
	int i = 0;
	int nerr = 0;
	while (tvs[i].str) {
		const struct testvector* tv = &tvs[i];

		// test 32 bit
		const unsigned long ch32 = hash_fnv32_s(tv->str, FNV32_INIT);
		if (ch32 != tv->h32) {
			fprintf(stderr, "Vector %d, mismatch for fnv32. "
			  "Got %lx, expected %lx.\n", i, ch32, tv->h32);
			++nerr;
		}

		// test 64 bit
		const unsigned long long int ch64 =
		  hash_fnv64_s(tv->str, FNV64_INIT);
		if (ch64 != tv->h64) {
			fprintf(stderr, "Vector %d, mismatch for fnv64. "
			  "Got %llx, expected %llx.\n", i, ch64, tv->h64);
			++nerr;
		}

		++i;
	}

	if (nerr == 0) {
		printf("All %d vectors passed.\n", i);
	} else {
		printf("%d failures seen.\n", nerr);
	}

	return nerr > 0 ? 1 : 0;
}
