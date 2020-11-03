#define CSNIP_SHORT_NAMES
#include <csnip/hash.h>

#define	FNV_p32		((uint32_t)0x01000193ul)
#define FNV_p64		((uint64_t)0x00000100000001B3ull)

#define hash_buf(buf, sz, h0, prime) \
	do { \
		const unsigned char* p = (const unsigned char*)buf; \
		while(sz > 0) { \
			h0 ^= *p; \
			h0 *= prime; \
			--sz; \
			++p; \
		} \
	} while(0)

#define hash_str(str, h0, prime) \
	do { \
		const unsigned char* p = (const unsigned char*)str; \
		while (*p) { \
			h0 ^= *p; \
			h0 *= prime; \
			++p; \
		} \
	} while (0)

uint32_t csnip_hash_fnv32_b(const void* buf, size_t sz, uint32_t h0)
{
	hash_buf(buf, sz, h0, FNV_p32);
	return h0;
}

uint32_t csnip_hash_fnv32_s(const char* str, uint32_t h0)
{
	hash_str(str, h0, FNV_p32);
	return h0;
}

uint64_t csnip_hash_fnv64_b(const void* buf, size_t sz, uint64_t h0)
{
	hash_buf(buf, sz, h0, FNV_p64);
	return h0;
}

uint64_t csnip_hash_fnv64_s(const char* str, uint64_t h0)
{
	hash_str(str, h0, FNV_p64);
	return h0;
}
