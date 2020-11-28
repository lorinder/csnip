#include <csnip/csnip_conf.h>
#ifdef CSNIP_HAVE_CORO_PTH

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#define CSNIP_SHORT_NAMES
#include <csnip/err.h>
#include <csnip/mem.h>
#undef CSNIP_SHORT_NAMES

#include <csnip/coro_pth.h>

typedef enum {
	ST_EP_UNSET,			/**< Entry point not configured */
	ST_EP_SET,			/**< EP set, not yet iterating */
	ST_ITER,			/**< iterating */
} coro_state;

typedef enum {
	RUN_MAIN,			/**< Main process is supposed to be running */
	RUN_CORO,			/**< The coroutine thread is supposed to be running */
} coro_running;

/** Coroutine state (pthreads based implementation) */
struct csnip_coro_pth_s {
	coro_state state;		/**< State */
	coro_running who;		/**< Which thread is currently running */

	pthread_t cothread;		/**< Coroutine thread */
	pthread_mutex_t mut;		/**< Synchronization mutex */
	pthread_cond_t cowake;		/**< Coroutine wake variable */
	pthread_cond_t mainwake;	/**< Main thread wake variable */

	csnip_coro_pth_func_ptr fn;		/**< Configured coroutine function */
	void* fn_arg;			/**< Configured coroutine argument */

	void* inout_ptr;		/**< Ptr passed to/from coroutine */
};

/** Checked call to a pthread_* function. */
#define PTHCHK(x) do { \
			int PTHCHK_err = (x); \
			if (PTHCHK_err != 0) { \
				errno = PTHCHK_err; \
				perror(#x); \
				exit(1); \
			} \
		} while(0)

static void thread_cancel(void* C_)
{
	csnip_coro_pth* C = C_;
	C->who = RUN_MAIN;
	PTHCHK(pthread_cond_signal(&C->mainwake));
	PTHCHK(pthread_mutex_unlock(&C->mut));
}

static void* thread_run(void* C_)
{
	csnip_coro_pth* C = C_;

	/* Set cleanup functions */
	pthread_cleanup_push(thread_cancel, C);

	/* Awake main thread;
	 * initially the main function will run.
	 */
	PTHCHK(pthread_mutex_lock(&C->mut));
	C->who = RUN_MAIN;
	PTHCHK(pthread_cond_signal(&C->mainwake));

	/* Iterate functions */
	while(1) {
		/* Wait for csnip_coro_pth_next() from main thread */
		do {
			PTHCHK(pthread_cond_wait(&C->cowake, &C->mut));
		} while (C->who != RUN_CORO);
		assert(C->state == ST_EP_SET);
		C->state = ST_ITER;
		PTHCHK(pthread_mutex_unlock(&C->mut));

		/* Start function */
		C->inout_ptr = (*C->fn)(C, C->fn_arg);
		C->state = ST_EP_SET;

		/* Hand control to main thread */
		PTHCHK(pthread_mutex_lock(&C->mut));
		C->who = RUN_MAIN;
		PTHCHK(pthread_cond_signal(&C->mainwake));
	}
	PTHCHK(pthread_mutex_unlock(&C->mut));

	pthread_cleanup_pop(0);
	return NULL;
}

csnip_coro_pth* csnip_coro_pth_new(void)
{
	int err = 0;
	csnip_coro_pth* C;
	csnip_mem_Alloc(1, C, err);
	if (err != 0)
		return NULL;

	/* Zero-initialize fields */
	C->fn = NULL;
	C->fn_arg = NULL;
	C->inout_ptr = NULL;
	C->state = ST_EP_UNSET;
	C->who = RUN_CORO;

	/* Create mutex */
	if ((err = pthread_mutex_init(&C->mut, NULL)) != 0) {
		mem_Free(C);
		return NULL;
	}

	/* Create condition variables */
	if ((err = pthread_cond_init(&C->cowake, NULL)) != 0) {
		pthread_mutex_destroy(&C->mut);
		mem_Free(C);
		return NULL;
	}
	if ((err = pthread_cond_init(&C->mainwake, NULL)) != 0) {
		pthread_cond_destroy(&C->cowake);
		pthread_mutex_destroy(&C->mut);
		mem_Free(C);
		return NULL;
	}

	/* Create the thread */
	PTHCHK(pthread_mutex_lock(&C->mut));
	if ((err = pthread_create(&C->cothread, NULL, thread_run, C)) != 0) {
		pthread_cond_destroy(&C->mainwake);
		pthread_cond_destroy(&C->cowake);
		pthread_mutex_destroy(&C->mut);
		mem_Free(C);
		return NULL;
	}
	do {
		pthread_cond_wait(&C->mainwake, &C->mut);
	} while (C->who != RUN_MAIN);
	PTHCHK(pthread_mutex_unlock(&C->mut));

	return C;
}

int csnip_coro_pth_set_func(csnip_coro_pth* C,
			csnip_coro_pth_func_ptr fptr,
			void* arg)
{
	/* Precondition */
	if (C->state == ST_ITER) {
		return csnip_err_CALLFLOW;
	}

	/* Set the function */
	C->fn = fptr;
	C->fn_arg = arg;
	C->state = ST_EP_SET;

	return 0;
}

int csnip_coro_pth_next(csnip_coro_pth* C, void* in, void** out)
{
	if (C->state == ST_EP_UNSET) {
		return csnip_err_CALLFLOW;
	}

	/* Advance coroutine */
	C->inout_ptr = in;
	PTHCHK(pthread_mutex_lock(&C->mut));
	C->who = RUN_CORO;
	PTHCHK(pthread_cond_signal(&C->cowake));
	do {
		PTHCHK(pthread_cond_wait(&C->mainwake, &C->mut));
	} while(C->who != RUN_MAIN);
	PTHCHK(pthread_mutex_unlock(&C->mut));

	if (out) {
		*out = C->inout_ptr;
	}
	return C->state != ST_ITER;
}

void* csnip_coro_pth_yield(csnip_coro_pth* C, void* val)
{
	C->inout_ptr = val;

	PTHCHK(pthread_mutex_lock(&C->mut));
	C->who = RUN_MAIN;
	PTHCHK(pthread_cond_signal(&C->mainwake));
	do {
		PTHCHK(pthread_cond_wait(&C->cowake, &C->mut));
	} while (C->who != RUN_CORO);
	PTHCHK(pthread_mutex_unlock(&C->mut));

	return C->inout_ptr;
}

void* csnip_coro_pth_get_value(csnip_coro_pth* C)
{
	return C->inout_ptr;
}

void csnip_coro_pth_free(csnip_coro_pth* C)
{
	/* Request cancellation, and wait for result */
	PTHCHK(pthread_mutex_lock(&C->mut));
	PTHCHK(pthread_cancel(C->cothread));
	do {
		PTHCHK(pthread_cond_wait(&C->mainwake, &C->mut));
	} while (C->who != RUN_MAIN);
	PTHCHK(pthread_mutex_unlock(&C->mut));

	/* Clean up */
	PTHCHK(pthread_mutex_destroy(&C->mut));
	PTHCHK(pthread_cond_destroy(&C->cowake));
	PTHCHK(pthread_cond_destroy(&C->mainwake));
	mem_Free(C);
}

#endif /* CSNIP_HAVE_CORO_PTH */
