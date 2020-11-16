#ifndef CSNIP_HASH_H
#define CSNIP_HASH_H

/** @file hash.h
 *  @addtogroup hash_functions	Hash functions
 *  @{
 *
 *  Hash functions
 *
 *  Defines good non-cryptographic hashing functions.
 *  Included are the 32 and 64 bit variants of the FNV-1a hash.
 *
 *  References:
 *	[1] https://tools.ietf.org/html/draft-eastlake-fnv-11
 *	[2] http://isthe.com/chongo/tech/comp/fnv/
 *
 *  Fairly interesting reads on the topic:
 *	http://programmers.stackexchange.com/questions/49550/\
 *	which-hashing-algorithm-is-best-for-uniqueness-and-speed
 */

#include <stdint.h>
#include <stddef.h>

/** FNV32's initialization constant. */
#define CSNIP_FNV32_INIT ((uint32_t)0x811C9DC5ul)

/** Compute FNV32 hash.
 *
 *  This computes the FNV hash of a memory buffer.
 *
 *  @param	buf
 *		Pointer to the buffer to hash
 *
 *  @param	sz
 *		Length of the buffer in bytes
 *
 *  @param	h0
 *		Initial hashing value.  If hashing just a single buffer,
 *		an arbitrary value can be used; the standard choice
 *		being CSNIP_FNV32_INIT.  The hash of a concatenation of
 *		buffers can be obtained by using this value for
 *		chaining, i.e.,
 *
 *			h1 = csnip_hash_fnv32_b(buf1, sz1, \
 *					CSNIP_FNV32_INIT);
 *			h = csnip_hash_fnv32_b(buf2, sz2, h1);
 *
 *		results in the same value of h as the computation
 *
 *			h = csnip_hash_fnv32_b(buf, sz, \
 *					CSNIP_FNV32_INIT);
 *
 *		assuming that buf is the concatenation of the buffers
 *		buf1 and buf2.
 *
 */
uint32_t csnip_hash_fnv32_b(const void* buf, size_t sz, uint32_t h0);

/** Compute FNV32 hash.
 *
 *  Compute the FNV32 hash of a C string.  The terminating '\0' is not
 *  itself included when the hash is computed.
 *
 *  @param	str
 *		C string to hash
 *
 *  @param	h0
 *		initial hash value. A good default is to use
 *		CSNIP_FNV32_INIT.  See csnip_hash_fnv32_b() about
 *		a way to hash a concatenation of strings.
 *
 */
uint32_t csnip_hash_fnv32_s(const char* str, uint32_t h0);

/** FNV64's initialization constant. */
#define CSNIP_FNV64_INIT ((uint64_t)0xCBF29CE484222325ull)

/** Compute FNV64 hash.
 *
 *  Compute the FNV64 hash of a memory buffer.
 *
 *  @param	buf
 *		pointer to memory buffer to hash
 *
 *  @param	sz
 *		size of the memory buffer to hash
 *
 *  @param	h0
 *		Initial value. Use, e.g., CSNIP_FNV64_INIT.  See the
 *		documentation of csnip_hash_fnv32_b() about hashing a
 *		concatenation of buffers.
 */
uint64_t csnip_hash_fnv64_b(const void* buf, size_t sz, uint64_t h0);

/** Compute FNV64 hash.
 *
 *  Compute the FNV64 hash of a C string.
 *
 *  @param	str
 *		string to hash.
 *
 *  @param	h0
 *		Initial value.  Use, e.g., CSNIP_FNV64_INIT.  See
 *		csnip_hash_fnv32_b() about hashing a concatenation of
 *		strings.
 */
uint64_t csnip_hash_fnv64_s(const char* str, uint64_t h0);

/** @} */

#endif /* CSNIP_HASH_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_HASH_HAVE_SHORT_NAMES)
#define FNV32_INIT	CSNIP_FNV32_INIT
#define FNV64_INIT	CSNIP_FNV64_INIT
#define hash_fnv32_b	csnip_hash_fnv32_b
#define hash_fnv32_s	csnip_hash_fnv32_s
#define hash_fnv64_b	csnip_hash_fnv64_b
#define hash_fnv64_s	csnip_hash_fnv64_s
#define CSNIP_HASH_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_HASH_HAVE_SHORT_NAMES */

