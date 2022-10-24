#include <time.h>

#include <csnip/csnip_conf.h>
#include <csnip/x.h>

#ifdef CSNIP_CONF__HAVE_CLOCK_GETTIME
int x_csnip_clock_gettime_imp(csnip_x_clockid_t clk_id,
			struct csnip_x_timespec* ts)
{
	return clock_gettime(clk_id, ts);
}
#elif defined(CSNIP_CONF__HAVE_TIMESPEC_GET)
int x_csnip_clock_gettime_imp(csnip_x_clockid_t clk_id,
			struct csnip_x_timespec* ts)
{
	if (clk_id == CSNIP_X_CLOCK_REALTIME) {
		const int r = timespec_get(ts, TIME_UTC);
		if (r == TIME_UTC)
			return 0;

		/* Didn't succeed;  we don't really know why not... */
		errno = EINVAL;
		return -1
	}

	/* Wrong clk_id */
	errno = EINVAL;
	return -1;
}
#endif
