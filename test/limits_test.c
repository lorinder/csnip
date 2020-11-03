#include <stdio.h>
#include <stdint.h>

#define CSNIP_SHORT_NAMES
#include <csnip/limits.h>

int main(void)
{
	printf("CHAR_MIN %d\n", limit_Mini((char)'a'));
	printf("CHAR_MAX %d\n", limit_Maxi((char)'a'));
	printf("SHRT_MIN %d\n", limit_Mini((short int)0));
	printf("SHRT_MAX %d\n", limit_Maxi((short int)0));
	printf("ULLONG_MAX %llu\n", limit_Maxi(1ull));
	return 0;
}
