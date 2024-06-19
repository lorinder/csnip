#include <assert.h>

#define CSNIP_SHORT_NAMES
#include <csnip/util.h>
#include <csnip/ringbuf2.h>

size_t ringbuf2_init(ringbuf2* rb, size_t min_cap)
{
	/* The actual capacity must be a power of 2 (for otherwise, a
	 * wrap in the n_written or n_read indices would corrupt the
	 * write/read position index.)
	 *
	 * We pick the next larger power of 2.
	 */
	const size_t cap = next_pow_of_2(min_cap);

	*rb = (ringbuf2) {
		.cap = cap,
		.n_written = 0,
		.n_read = 0
	};

	return cap;
}

ringbuf2 ringbuf2_make(size_t min_cap)
{
	ringbuf2 rb;
	ringbuf2_init(&rb, min_cap);
	return rb;
}

size_t ringbuf2_used_size(const ringbuf2* rb)
{
	return rb->n_written - rb->n_read;
}

size_t ringbuf2_free_size(const ringbuf2* rb)
{
	return rb->cap - ringbuf2_used_size(rb);
}

size_t ringbuf2_get_write_idx(const ringbuf2* rb, size_t* ret_contig_write_max)
{
	const size_t write_idx = rb->n_written & (rb->cap - 1);
	if (ret_contig_write_max) {
		const size_t n_free = ringbuf2_free_size(rb);
		const size_t n_to_end = rb->cap - write_idx;
		*ret_contig_write_max = Min(n_free, n_to_end);
	}
	return write_idx;
}

bool ringbuf2_add_written(ringbuf2* rb, size_t n_written)
{
	rb->n_written += n_written;
	return rb->n_written - rb->n_read <= rb->cap;
}

size_t ringbuf2_get_read_idx(const ringbuf2* rb, size_t* ret_contig_read_max)
{
	const size_t read_idx = rb->n_read & (rb->cap - 1);
	if (ret_contig_read_max) {
		const size_t n_used = ringbuf2_used_size(rb);
		const size_t n_to_end = rb->cap - read_idx;
		*ret_contig_read_max = Min(n_used, n_to_end);
	}
	return read_idx;
}

bool ringbuf2_add_read(ringbuf2* rb, size_t n_read)
{
	rb->n_read += n_read;
	return rb->n_read <= rb->n_written;
}
