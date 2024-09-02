#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CSNIP_SHORT_NAMES
#include <csnip/ringbuf2.h>
#include <csnip/util.h>

#define CHECK(x) \
	do { \
		if (!(x)) { \
			printf(" FAIL\n"); \
			fprintf(stderr, "check \"%s\" failed\n", #x); \
			exit(1); \
		} \
	} while (0)

static void test_init_k(int k, int expect)
{
	ringbuf2 rb;
	memset(&rb, 'x', sizeof rb);

	CHECK(ringbuf2_init(&rb, k) == expect);
	CHECK(rb.cap == expect);
	CHECK(rb.n_written == 0);
	CHECK(rb.n_read == 0);

	ringbuf2 rb2 = ringbuf2_make(k);
	CHECK(rb2.cap == expect);
	CHECK(rb2.n_written == 0);
	CHECK(rb2.n_read == 0);
}

void test_init(void)
{
	test_init_k(15, 16);
	test_init_k(16, 16);
	test_init_k(17, 32);
}

/* Create example ring buffers
 *
 * - straight has the write index after read
 * - twisted has the write index before read
 */

size_t rand_i(int i)
{
	size_t ii = i;
	for (int j = 0; j < 8; ++j) {
		ii *= 1234567u;
		ii ^= ii >> 12;
	}
	return ii;
}

ringbuf2 get_rb2_straight(int i, size_t* ret_widx, size_t* ret_ridx)
{
	/* Capacity */
	const size_t cap = ((size_t)1) << (rand_i(i) & 31);

	/* Indices */
	size_t a = rand_i(i + 1) % cap;
	size_t b = rand_i(i + 2) % cap;
	const size_t widx = Max(a, b);
	const size_t ridx = Min(a, b);

	ringbuf2 rb = {
		.cap = cap,
		.n_written = widx + i * cap,
		.n_read = ridx + i * cap,
	};

	*ret_widx = widx;
	*ret_ridx = ridx;
	return rb;
}

ringbuf2 get_rb2_twisted(int i, size_t* ret_widx, size_t* ret_ridx)
{
	size_t widx0, ridx0;
	ringbuf2 rb_s = get_rb2_straight(i, &widx0, &ridx0);
	ringbuf2 rb = (ringbuf2) {
		.cap = rb_s.cap,
		.n_written = ridx0 + rb_s.cap,
		.n_read = widx0,
	};

	*ret_widx = ridx0;
	*ret_ridx = widx0;
	return rb;
}

void straighttwisted_checks(void)
{
	for (int i = 0; i < 1024; ++i) {
		/* Straight checks */
		printf("Straight checks, i = %d\n", i);
		size_t widx, ridx;
		ringbuf2 rb = get_rb2_straight(i, &widx, &ridx);
		CHECK(ringbuf2_used_size(&rb) == widx - ridx);
		CHECK(ringbuf2_free_size(&rb)
			== rb.cap - ringbuf2_used_size(&rb));
		CHECK(ringbuf2_get_write_idx(&rb, NULL) == widx);
		CHECK(ringbuf2_get_read_idx(&rb, NULL) == ridx);
		size_t n_contig;
		CHECK(ringbuf2_get_write_idx(&rb, &n_contig) == widx);
		CHECK(n_contig == rb.cap - widx);
		CHECK(ringbuf2_get_read_idx(&rb, &n_contig) == ridx);
		CHECK(n_contig == widx - ridx);
		{
			/* Test ringbuf2_get_read_areas() */
			size_t pos[2], len[2];
			int n = ringbuf2_get_read_areas(&rb,
					&pos[0], &len[0],
					&pos[1], &len[1]);
			CHECK(n == 0 || n == 1);
			size_t used_sz = ringbuf2_used_size(&rb);
			if (n == 0) {
				CHECK(used_sz == 0);
			} else {
				CHECK(len[0] > 0);
				CHECK(len[0] == used_sz);
				size_t cspc;
				size_t ri = ringbuf2_get_read_idx(&rb, &cspc);
				CHECK(cspc == len[0]);
				CHECK(ri == pos[0]);
			}
		}
		{
			/* Test ringbuf2_get_write_areas() */
			size_t pos[2], len[2];
			int n = ringbuf2_get_write_areas(&rb,
					&pos[0], &len[0],
					&pos[1], &len[1]);
			size_t free_sz = ringbuf2_free_size(&rb);
			size_t cspc;
			size_t wi = ringbuf2_get_write_idx(&rb, &cspc);
			switch (n) {
			case 0:
				CHECK(free_sz == 0);
				break;
			case 1:
				CHECK(len[0] > 0);
				CHECK(len[0] == free_sz);
				CHECK(cspc == len[0]);
				CHECK(wi == pos[0]);
				break;
			case 2:
				CHECK(len[0] > 0);
				CHECK(len[1] > 0);
				CHECK(len[0] + len[1] == free_sz);
				CHECK(cspc == len[0]);
				CHECK(cspc <= free_sz);
				CHECK(wi == pos[0]);
				CHECK(pos[1] == 0);
				break;
			};
		}
		if (ringbuf2_free_size(&rb) > 0) {
			CHECK(ringbuf2_add_written(&rb, 1));
			CHECK(ringbuf2_get_write_idx(&rb, NULL)
				== (widx + 1) % rb.cap);
		}
		if (ringbuf2_used_size(&rb) > 0) {
			CHECK(ringbuf2_add_read(&rb, 1));
			CHECK(ringbuf2_get_read_idx(&rb, NULL)
				== (ridx + 1) % rb.cap);
		}

		/* Twisted checks */
		printf("Twisted checks, i = %d\n", i);
		rb = get_rb2_twisted(i, &widx, &ridx);
		CHECK(ringbuf2_used_size(&rb) == rb.cap + widx - ridx);
		CHECK(ringbuf2_free_size(&rb) == ridx - widx);
		CHECK(ringbuf2_get_write_idx(&rb, NULL) == widx);
		CHECK(ringbuf2_get_read_idx(&rb, NULL) == ridx);
		CHECK(ringbuf2_get_write_idx(&rb, &n_contig) == widx);
		CHECK(n_contig == ridx - widx);
		CHECK(ringbuf2_get_read_idx(&rb, &n_contig) == ridx);
		CHECK(n_contig == rb.cap - ridx);
		{
			/* Test ringbuf2_get_read_areas() */
			size_t pos[2], len[2];
			int n = ringbuf2_get_read_areas(&rb,
					&pos[0], &len[0],
					&pos[1], &len[1]);
			size_t used_sz = ringbuf2_used_size(&rb);
			size_t cspc;
			size_t ri = ringbuf2_get_read_idx(&rb, &cspc);
			switch (n) {
			case 0:
				CHECK(used_sz == 0);
				break;
			case 1:
				CHECK(len[0] > 0);
				CHECK(len[0] == used_sz);
				CHECK(cspc == len[0]);
				CHECK(ri == pos[0]);
				break;
			case 2:
				CHECK(len[0] > 0);
				CHECK(len[1] > 0);
				CHECK(len[0] + len[1] == used_sz);
				CHECK(cspc == len[0]);
				CHECK(cspc <= used_sz);
				CHECK(ri == pos[0]);
				CHECK(pos[1] == 0);
				break;
			};
		}
		{
			/* Test ringbuf2_get_write_areas() */
			size_t pos[2], len[2];
			int n = ringbuf2_get_write_areas(&rb,
					&pos[0], &len[0],
					&pos[1], &len[1]);
			CHECK(n == 0 || n == 1);
			size_t free_sz = ringbuf2_free_size(&rb);
			if (n == 0) {
				CHECK(free_sz == 0);
			} else {
				CHECK(len[0] > 0);
				CHECK(len[0] == free_sz);
				size_t cspc;
				size_t wi = ringbuf2_get_write_idx(&rb, &cspc);
				CHECK(cspc == len[0]);
				CHECK(wi == pos[0]);
			}
		}
		if (ringbuf2_free_size(&rb) > 0) {
			CHECK(ringbuf2_add_written(&rb, 1));
			CHECK(ringbuf2_get_write_idx(&rb, NULL)
				== (widx + 1) % rb.cap);
		}
		if (ringbuf2_used_size(&rb) > 0) {
			CHECK(ringbuf2_add_read(&rb, 1));
			CHECK(ringbuf2_get_read_idx(&rb, NULL)
				== (ridx + 1) % rb.cap);
		}
	}
}


int main(int argc, char** argv)
{
	test_init();
	straighttwisted_checks();
	return 0;
}
