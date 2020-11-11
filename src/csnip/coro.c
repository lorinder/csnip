#include <stdint.h>
#include <string.h>

#include <ucontext.h>

#define CSNIP_SHORT_NAMES
#include <csnip/err.h>
#include <csnip/mem.h>
#undef CSNIP_SHORT_NAMES

#include "coro.h"

typedef enum {
	/** No entry point set yet.
	 *
	 *  This is the state of a freshly created coro before the entry
	 *  point has been set, and also the state of a coro after it
	 *  terminated.
	 */
	ST_EP_UNSET,

	/** Coroutine entry point has been set.
	 *
	 *  The coroutine has an assigned entry point, but is not yet
	 *  iterating.
	 */
	ST_FUNC_SET,

	/** Coroutine is iterating. */
	ST_ITER,
} coro_state;

struct csnip_coro_s {
	/** State of the coroutine */
	coro_state state;

	/** Context of the main routine. */
	ucontext_t main_ctx;

	/** Context of the coroutine. */
	ucontext_t co_ctx;

	/** Stack memory of the coroutine. */
	char* stack;

	/** Stack size of the coroutine. */
	int stack_size;

	/** Coroutine entry point */
	csnip_coro_func_ptr fptr;

	/** Entry point argument */
	void* farg;

	/* Pointer from / to the coroutine */
	void* inout_ptr;
};

csnip_coro* csnip_coro_new(void)
{
	/* Alloc the coro */
	int err = 0;
	csnip_coro* C;
	mem_Alloc(1, C, err);
	if (err != 0)
		return NULL;

	/* Alloc the stack */
	C->stack_size = 16384;
	mem_Alloc(C->stack_size, C->stack, err);
	if (err != 0) {
		mem_Free(C);
		return NULL;
	}

	C->state = ST_EP_UNSET;
	C->inout_ptr = NULL;

	/* The other members, in partuclar the contexts,
	 * and entry points, are set in csnip_coro_set_func().
	 */

	return C;
}

/**	Union to convert a pointer into an array of uint16_t.
 *
 *	This is used to encode the argument pointer when passing it via
 *	makecontext().
 */
union ptr2u16 {
	void* ptr;
	uint16_t i[4];
};

static void starter_func2(void* C_);

#if UINTPTR_MAX <= 0xFFFFFFFF
static void starter_func(int v0, int v1)
{
	union ptr2u16 un;
	un.ptr = 0;
	un.i[0] = v0;
	un.i[1] = v1;
	starter_func2(un.ptr);
}
#elif UINTPTR_MAX <= 0xFFFFFFFFFFFFFFFF
static void starter_func(int v0, int v1, int v2, int v3)
{
	union ptr2u16 un;
	un.ptr = 0;
	un.i[0] = v0;
	un.i[1] = v1;
	un.i[2] = v2;
	un.i[3] = v3;
	starter_func2(un.ptr);
}
#else
#error Unsupported pointer size
#endif

static void starter_func2(void* C_)
{
	csnip_coro* C = C_;
	void* ret = (*C->fptr)(C, C->farg);
	C->inout_ptr = ret;
	C->state = ST_EP_UNSET;
}

int csnip_coro_set_func(csnip_coro* C, csnip_coro_func_ptr fptr, void* arg)
{
	/* Precondition checks */
	if (C->state != ST_EP_UNSET && C->state != ST_FUNC_SET) {
		return csnip_err_CALLFLOW;
	}

	/* Create the context */
	if (getcontext(&C->co_ctx) != 0)
		return err_ERRNO;
	stack_t st = {
		.ss_sp = C->stack,
		.ss_flags = 0,
		.ss_size = C->stack_size,
	};
	C->co_ctx.uc_stack = st;
	C->co_ctx.uc_link = &C->main_ctx;

	union ptr2u16 un;
	memset(&un, 0, sizeof(un));
	un.ptr = C;
#if UINTPTR_MAX <= 0xFFFFFFFF
	/* <= 32 bit pointers */
	makecontext(&C->co_ctx, (void (*)(void))starter_func,
		2, un.i[0], un.i[1]);
#elif UINTPTR_MAX <= 0xFFFFFFFFFFFFFFFF
	/* <= 64 bit pointers */
	makecontext(&C->co_ctx, (void (*)(void))starter_func,
		4, un.i[0], un.i[1], un.i[2], un.i[3]);
#else
#error Unsupported pointer size.
#endif

	C->fptr = fptr;
	C->farg = arg;
	C->inout_ptr = NULL;

	C->state = ST_FUNC_SET;

	return 0;
}

int csnip_coro_next(csnip_coro* C, void* in, void** out)
{
	if (C->state == ST_EP_UNSET) {
		return csnip_err_CALLFLOW;
	}

	/* Jump into coroutine */
	C->state = ST_ITER;
	C->inout_ptr = in;
	swapcontext(&C->main_ctx, &C->co_ctx);
	if (out) {
		*out = C->inout_ptr;
	}
	return (C->state != ST_ITER);
}

void* csnip_coro_yield(csnip_coro* C, void* val)
{
	C->inout_ptr = val;
	swapcontext(&C->co_ctx, &C->main_ctx);
	return C->inout_ptr;
}

void* csnip_coro_get_value(csnip_coro* C)
{
	return C->inout_ptr;
}

void csnip_coro_free(csnip_coro* C)
{
	mem_Free(C->stack);
	mem_Free(C);
}
