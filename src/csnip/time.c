#include <sys/time.h>

#include "time.h"

typedef long double ldouble;

#define DEF_SCALAR2TS_FUNC(scalar_type) \
	struct timespec csnip_time_ ## scalar_type ## _as_timespec( \
			scalar_type f) \
	{ \
		struct timespec ret; \
		ret.tv_sec = f; \
		ret.tv_nsec = (f - ret.tv_sec) * 1e9; \
		return ret; \
	}

DEF_SCALAR2TS_FUNC(time_t)
DEF_SCALAR2TS_FUNC(float)
DEF_SCALAR2TS_FUNC(double)
DEF_SCALAR2TS_FUNC(ldouble)

#undef DEF_SCALAR2TS_FUNC

struct timespec csnip_time_timeval_as_timespec(struct timeval tv)
{
	struct timespec ret;
	ret.tv_sec = tv.tv_sec;
	ret.tv_nsec = tv.tv_usec * 1000;
	return ret;
}

#define DEF_TS2SCALAR_FUNC(scalar_type) \
	scalar_type csnip_time_timespec_as_ ## scalar_type( \
			const struct timespec ts) \
	{ \
		return ts.tv_sec + (ts.tv_nsec / 1e9); \
	}

DEF_TS2SCALAR_FUNC(time_t)
DEF_TS2SCALAR_FUNC(float)
DEF_TS2SCALAR_FUNC(double)
DEF_TS2SCALAR_FUNC(ldouble)

#undef DEF_TS2SCALAR_FUNC

struct timeval csnip_time_timespec_as_timeval(struct timespec ts)
{
	struct timeval ret;
	ret.tv_sec = ts.tv_sec;
	ret.tv_usec = ts.tv_nsec / 1000;
	return ret;
}

int csnip_time_sleep(struct timespec ts)
{
	int err = nanosleep(&ts, NULL);
	if (err != 0) {
		return csnip_err_ERRNO;
	}
	return 0;
}

int csnip_time_is_less(struct timespec a, struct timespec b)
{
	if (a.tv_sec < b.tv_sec)
		return 1;
	if (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec)
		return 1;
	return 0;
}

int csnip_time_is_less_equal(struct timespec a, struct timespec b)
{
	if (a.tv_sec < b.tv_sec)
		return 1;
	if (a.tv_sec == b.tv_sec && a.tv_nsec <= b.tv_nsec)
		return 1;
	return 0;
}

struct timespec csnip_time_add(struct timespec a, struct timespec b)
{
	a.tv_sec += b.tv_sec;
	a.tv_nsec += b.tv_nsec;
	if (a.tv_nsec >= 1000000000l) {
		++a.tv_sec;
		a.tv_nsec -= 1000000000l;
	} else if (a.tv_nsec < 0) {
		--a.tv_sec;
		a.tv_nsec += 1000000000l;
	}
	return a;
}

struct timespec csnip_time_sub(struct timespec a, struct timespec b)
{
	a.tv_sec -= b.tv_sec;
	a.tv_nsec -= b.tv_nsec;
	if (a.tv_nsec >= 1000000000l) {
		++a.tv_sec;
		a.tv_nsec -= 1000000000l;
	} else if (a.tv_nsec < 0) {
		--a.tv_sec;
		a.tv_nsec += 1000000000l;
	}
	return a;
}
