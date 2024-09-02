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

int csnip_ringbuf2_get_write_areas(const ringbuf2* rb,
				size_t* ret_idx_0,
				size_t* ret_len_0,
				size_t* ret_idx_1,
				size_t* ret_len_1)
{
	/* Buffer full? */
	if (rb->n_written - rb->n_read >= rb->cap)
		return 0;

	/* Figure out geometry.
	 *
	 * If rdi <= wri, then there is free space wrap around,
	 * i.e., there will be two contiguous write sections.
	 *
	 * Otherwise, there is a single section.
	 */
	const size_t wri = rb->n_written & (rb->cap - 1);
	const size_t rdi = rb->n_read & (rb->cap - 1);

	if (ret_idx_0)
		*ret_idx_0 = wri;

	if (rdi <= wri) {
		if (ret_len_0)
			*ret_len_0 = rb->cap - wri;
		if (rdi > 0) {
			if (ret_idx_1)
				*ret_idx_1 = 0;
			if (ret_len_1)
				*ret_len_1 = rdi;
			return 2;
		} else {
			return 1;
		}
	} else {
		if (ret_len_0)
			*ret_len_0 = rdi - wri;
		return 1;
	}
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

int csnip_ringbuf2_get_read_areas(const ringbuf2* rb,
				size_t* ret_idx_0,
				size_t* ret_len_0,
				size_t* ret_idx_1,
				size_t* ret_len_1)
{
	/* Buffer empty? */
	if (rb->n_written - rb->n_read == 0)
		return 0;

	/* Figure out geometry.
	 *
	 * If wri <= rdi, then there is read space wrap around,
	 * i.e., there will be two contiguous read sections.
	 *
	 * Otherwise, there is a single section.
	 */
	const size_t wri = rb->n_written & (rb->cap - 1);
	const size_t rdi = rb->n_read & (rb->cap - 1);

	if (ret_idx_0)
		*ret_idx_0 = rdi;

	if (wri <= rdi) {
		if (ret_len_0)
			*ret_len_0 = rb->cap - rdi;
		if (wri > 0) {
			if (ret_idx_1)
				*ret_idx_1 = 0;
			if (ret_len_1)
				*ret_len_1 = wri;
			return 2;
		} else {
			return 1;
		}
	} else {
		if (ret_len_0)
			*ret_len_0 = wri - rdi;
		return 1;
	}
}

bool ringbuf2_add_read(ringbuf2* rb, size_t n_read)
{
	rb->n_read += n_read;
	return rb->n_read <= rb->n_written;
}
