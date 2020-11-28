#include <csnip/csnip_conf.h>
#ifdef CSNIP_HAVE_CORO_UCTX

#ifndef CSNIP_CORO_UCTX_H
#define CSNIP_CORO_UCTX_H

#ifdef __cplusplus
extern "C" {
#endif

struct csnip_coro_uctx_s;
typedef struct csnip_coro_uctx_s csnip_coro_uctx;
typedef void* (*csnip_coro_uctx_func_ptr)(csnip_coro_uctx*, void*);
csnip_coro_uctx* csnip_coro_uctx_new(void);
int csnip_coro_uctx_set_func(csnip_coro_uctx* C, csnip_coro_uctx_func_ptr fptr, void* arg);
int csnip_coro_uctx_next(csnip_coro_uctx* C, void* in, void** out);
void* csnip_coro_uctx_yield(csnip_coro_uctx* C, void* val);
void* csnip_coro_uctx_get_value(csnip_coro_uctx* C);
void csnip_coro_uctx_free(csnip_coro_uctx* C);

#ifdef __cplusplus
}
#endif

#endif /* CSNIP_CORO_UCTX_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_CORO_CTX_HAVE_SHORT_NAMES)
#define coro_uctx_s		csnip_coro_uctx_s
#define	coro_uctx		csnip_coro_uctx
#define coro_uctx_func_ptr	csnip_coro_uctx_func_ptr
#define coro_uctx_new		csnip_coro_uctx_new
#define coro_uctx_set_func	csnip_coro_uctx_set_func
#define coro_uctx_next		csnip_coro_uctx_next
#define coro_uctx_yield		csnip_coro_uctx_yield
#define coro_uctx_get_value	csnip_coro_uctx_get_value
#define coro_uctx_free		csnip_coro_uctx_free
#define CSNIP_CORO_CTX_HAVE_SHORT_NAMES
#endif

#endif /* CSNIP_HAVE_CORO_UCTX */
