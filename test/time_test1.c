#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csnip/time.h>
#include <csnip/x.h>

static void check_ts(struct timespec ts,
			time_t secs,
			long int nsecs)
{
	printf("Computed ts: tv_sec = %ld, tv_nsec = %ld ",
		(long int)ts.tv_sec,
		(long int)ts.tv_nsec);
	if (secs != ts.tv_sec || nsecs != ts.tv_nsec) {
		fflush(stdout);
		fprintf(stderr, "-> Fail, expected tv_sec = %ld, "
		  "tv_nsec = %ld\n", (long)secs, (long)nsecs);
		exit(1);
	}
	puts("-> OK");
}

void conversion_tests()
{
	struct timespec ts;
	ts = csnip_time_AsTimespec(7.5);
	check_ts(ts, 7, 500000000);

	struct timeval tv = { 2, 300000 };
	ts = csnip_time_AsTimespec(tv);
	check_ts(ts, 2, 300000000);

	ts = csnip_time_Add(1.1, 2.2);
	check_ts(ts, 3, 300000000);

	double s;
	csnip_time_Convert(ts, s);
	printf("as double: %g ", s);
	if (s < 3.3 - 0.001 || s > 3.3 + 0.001) {
		fflush(stdout);
		fprintf(stderr, "-> Fail, expected 3.3\n");
		exit(1);
	}
	puts("-> OK");
}

void func_tests()
{
	/* Smoke test for the sleep function */
	{
		struct csnip_x_timespec t0;
		csnip_x_clock_gettime(CSNIP_X_CLOCK_REALTIME, &t0);
		csnip_time_Sleep(0.1, _);
		struct timespec t1;
		csnip_x_clock_gettime(CSNIP_X_CLOCK_REALTIME, &t1);
		double diff;
		csnip_time_Convert(csnip_time_Sub(t1, t0), diff);
		printf("After sleeping 0.1s, got delta of %gs. ", diff);
		/* Ideally, we would like to check that diff is not
		 * too much larger than 0.1, but on a heavily loaded
		 * system there is no bound on how much larger it really
		 * can be.
		 */
		if (diff < 0.1) {
			fflush(stdout);
			fprintf(stderr, "-> FAIL\n");
			exit(1);
		} else {
			puts("-> ok\n");
		}
	}

	/* Trying to do some time adding */
	time_t base = 1583730888;
	printf("Base time: %ld  %s", (long int)base, asctime(gmtime(&base)));

	time_t baseplus5;
	csnip_time_Convert(csnip_time_Add(base, 5.4), baseplus5);
	char* ascbp5 = strdup(asctime(gmtime(&baseplus5)));
	printf("+5s  time: %ld  %s", (long int)baseplus5, ascbp5);
	if (strcmp(ascbp5, "Mon Mar  9 05:14:53 2020\n") != 0
#if defined(WIN32)
		/* Microsoft's libc generates a string that is not quite
		 * conforming to the standard.  This is not our bug, so
		 * no reason to fail the test suite for this reason.
		 */
		&& strcmp(ascbp5, "Mon Mar 09 05:14:53 2020\n") != 0
#endif
	) {
		fprintf(stderr, "Error: Comparison failed.\n");
		exit(1);
	}
	free(ascbp5);
	puts("-> OK");

	/* Subtraction */
	struct timespec diff = csnip_time_Sub(baseplus5, base);
	check_ts(diff, 5, 0); // XXX: This check assumes time_t has 1s resolution.
}

int main(int argc, char** argv)
{
	conversion_tests();
	func_tests();
	return 0;
}
