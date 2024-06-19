#ifndef CSNIP_RINGBUF2_H
#define CSNIP_RINGBUF2_H

/**	@file ringbuf2.h
 *	@brief				Ring buffers, v2
 *	@defgroup ringbuf2		Ring buffers, v2
 *	@{
 *
 *	@brief Ring buffer implementation, v2
 *
 *	Implements a simple ring buffer with a capacity of a power of 2.
 *	This module only manages the indices, the user needs to keep a
 *	backing array separately.
 *
 *	This module is intended as a replacement for csnip's first
 *	ringbuf module;  the former has been found to be clunky to use
 *	in practice.
 *
 *	No type generic macros are provided here, as the data structure
 *	has a fixed type.
 *
 *	The data structure used here, making use of controlled unsigned
 *	integer overflow is inspired by io_uring's communication ring
 *	buffers, see https://kernel.dk/io_uring.pdf
 */

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	/** buffer capacity.
	 *
	 *  Must be a power of 2.
	 */
	size_t cap;

	/** Number of elements written so far.
	 *
	 *  The number is mod (SIZE_MAX + 1), i.e., using C's
	 *  wrapping unsigned integer arithmetic.
	 */
	size_t n_written;

	/** Number of elements read so far.
	 *
	 *  The number is mod (SIZE_MAX + 1), using C's wrapping
	 *  unsigned integer arithmetic.
	 */
	size_t n_read;
} csnip_ringbuf2;

size_t csnip_ringbuf2_init(csnip_ringbuf2* rb, size_t min_cap);
csnip_ringbuf2 csnip_ringbuf2_make(size_t min_cap);

size_t csnip_ringbuf2_used_size(const csnip_ringbuf2* rb);
size_t csnip_ringbuf2_free_size(const csnip_ringbuf2* rb);

size_t csnip_ringbuf2_get_write_idx(const csnip_ringbuf2* rb,
				size_t* ret_contig_write_max);
bool csnip_ringbuf2_add_written(csnip_ringbuf2* rb, size_t n_written);

size_t csnip_ringbuf2_get_read_idx(const csnip_ringbuf2* rb,
				size_t* ret_contig_read_max);
bool csnip_ringbuf2_add_read(csnip_ringbuf2* rb, size_t n_read);

#endif /* CSNIP_RINGBUF2_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_RINGBUF2_HAVE_SHORT_NAMES)
#define ringbuf2		csnip_ringbuf2
#define ringbuf2_init		csnip_ringbuf2_init
#define ringbuf2_make		csnip_ringbuf2_make
#define ringbuf2_used_size	csnip_ringbuf2_used_size
#define ringbuf2_free_size	csnip_ringbuf2_free_size
#define ringbuf2_get_write_idx	csnip_ringbuf2_get_write_idx
#define ringbuf2_add_written	csnip_ringbuf2_add_written
#define ringbuf2_get_read_idx	csnip_ringbuf2_get_read_idx
#define ringbuf2_add_read	csnip_ringbuf2_add_read
#define CSNIP_RINGBUF2_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_RINGBUF2_HAVE_SHORT_NAMES */
