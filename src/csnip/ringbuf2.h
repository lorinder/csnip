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

/**	Ringbuffer type.
 *
 *	Stores the capacity of the ring buffer and the positions of
 *	reader & writer.
 */
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

/**	Initialize a ringbuffer.
 *
 *	@param	min_cap
 *		the minimum required capacity of the ring buffer.  Since
 *		the ringbuffer size must be a power of 2, the actual
 *		capacity used will be this value rounded up to the next
 *		power of 2.
 *
 *	@return	the used capacity.
 */
size_t csnip_ringbuf2_init(csnip_ringbuf2* rb, size_t min_cap);

/**	Create a ringbuffer.
 *
 *	Returns an initialized ringbuffer of the given minimum
 *	capacity.  Can be used instead of csnip_ringbuf2_init() if
 *	initializer syntax is more convenient.
 *
 *	@param	min_cap
 *		the minimum capacity.  The actual value will be rounded
 *		up to the next power of 2, @sa csnip_ringbuf2_init()
 *
 *	@sa csnip_ringbuf2_init()
 */
csnip_ringbuf2 csnip_ringbuf2_make(size_t min_cap);

/**	Return the number of occupied entries.
 *
 *	Returns the number of ring buffer entries that are occupied,
 *	i.e., written, but not yet consumed with write().
 */
size_t csnip_ringbuf2_used_size(const csnip_ringbuf2* rb);

/**	Return the number of unoccupied entries.
 *
 *	Returns the number of ring buffer entries that are unoccupied,
 *	i.e., that can still be written with out any reads until the
 *	buffer is full.
 */
size_t csnip_ringbuf2_free_size(const csnip_ringbuf2* rb);

/**	Get the write position in the ring buffer.
 *
 *	Retrieve the next index in the ring buffer that can be written
 *	to, and also, if desired, the amount of contiguous space left at
 *	that position.
 *
 *	@param	rb
 *		the ring buffer
 *
 *	@param	ret_contig_write_max
 *		if non-NULL the number of contiguous slots available at
 *		the write position will be returned into
 *		*ret_contig_write_max.
 *
 *	@return	next writable index.  Note that calling this function on
 *		a full buffer is not an error, but writing to that index
 *		would then overwrite a previous, not yet read entry.
 */
size_t csnip_ringbuf2_get_write_idx(const csnip_ringbuf2* rb,
				size_t* ret_contig_write_max);

/**	Return the contiguous writable areas.
 *
 *	Return the (at most 2) contiguous areas that are writable, and
 *	whose writing in order does fill the ring buffer in that order.
 *
 *	@param	ret_idx_0
 *		if non-NULL, the start index of the first area will be
 *		returned here, if applicable.
 *
 *	@param	ret_len_0
 *		if non-NULL, the length of the first area will be
 *		returned here, if applicable.
 *
 *	@param	ret_idx_1, ret_len_1
 *		same for the second area, if applicable.
 *
 *	@return	the number of areas:
 *		- 0 if the buffer is full and there are no writable
 *		  positions.
 *		- 1 if the buffer can be filled by writing into a single
 *		  contiguous area.  Only non-NULL ret_idx_0 and ret_len_0
 *		  will be set in that case.
 *		- 2 if two contiguous areas are needed to fill the
 *		  buffer completely.
 */
int csnip_ringbuf2_get_write_areas(const csnip_ringbuf2* rb,
				size_t* ret_idx_0,
				size_t* ret_len_0,
				size_t* ret_idx_1,
				size_t* ret_len_1);

/**	Update ringbuffer after adding entries.
 *
 *	This marks positions in the ring buffer as full.  In case that
 *	the buffer was overflowed, the oldest entries in the buffer were
 *	overwritten and they're silently discarded.
 *
 *	@param	rb
 *		Ringbuffer
 *
 *	@param	n_written
 *		number of items that were written.
 *
 *	@return	true	if the update was successful
 *		false	if the update resulted in overflowing the
 *			buffer.
 */
bool csnip_ringbuf2_add_written(csnip_ringbuf2* rb, size_t n_written);

/**	Get the next read position in the ring buffer.
 *
 *	This is the analog to csnip_ringbuf2_get_write_idx() for reads.
 *	@sa csnip_ringbuf2_get_write_idx() for details.
 */
size_t csnip_ringbuf2_get_read_idx(const csnip_ringbuf2* rb,
				size_t* ret_contig_read_max);

/**	Get the contiguous readable areas.
 *
 *	This is the analog to csnip_ringbuf2_get_write_areas() for reads.
 *	@sa csnip_ringbuf2_get_write_areas() for details.
 */
int csnip_ringbuf2_get_read_areas(const csnip_ringbuf2* rb,
				size_t* ret_idx_0,
				size_t* ret_len_0,
				size_t* ret_idx_1,
				size_t* ret_len_1);

/**	Update the ringbuffer after reading entries.
 *
 *	Analog to csnip_ringbuf2_add_written().  @sa
 *	csnip_ringbuf2_add_written().
 */
bool csnip_ringbuf2_add_read(csnip_ringbuf2* rb, size_t n_read);

#endif /* CSNIP_RINGBUF2_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_RINGBUF2_HAVE_SHORT_NAMES)
#define ringbuf2			csnip_ringbuf2
#define ringbuf2_init			csnip_ringbuf2_init
#define ringbuf2_make			csnip_ringbuf2_make
#define ringbuf2_used_size		csnip_ringbuf2_used_size
#define ringbuf2_free_size		csnip_ringbuf2_free_size
#define ringbuf2_get_write_idx		csnip_ringbuf2_get_write_idx
#define ringbuf2_get_write_areas	csnip_ringbuf2_get_write_areas
#define ringbuf2_add_written		csnip_ringbuf2_add_written
#define ringbuf2_get_read_idx		csnip_ringbuf2_get_read_idx
#define ringbuf2_get_read_areas		csnip_ringbuf2_get_read_areas
#define ringbuf2_add_read		csnip_ringbuf2_add_read
#define CSNIP_RINGBUF2_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_RINGBUF2_HAVE_SHORT_NAMES */
