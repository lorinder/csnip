#include <csnip/csnip_conf.h>
#ifdef CSNIP_HAVE_CORO_PTH

#ifndef CSNIP_CORO_PTH_H
#define CSNIP_CORO_PTH_H

#ifdef __cplusplus
extern "C" {
#endif

struct csnip_coro_pth_s;
typedef struct csnip_coro_pth_s csnip_coro_pth;
typedef void* (*csnip_coro_pth_func_ptr)(csnip_coro_pth*, void*);
csnip_coro_pth* csnip_coro_pth_new(void);
int csnip_coro_pth_set_func(csnip_coro_pth* C, csnip_coro_pth_func_ptr fptr, void* arg);
int csnip_coro_pth_next(csnip_coro_pth* C, void* in, void** out);
void* csnip_coro_pth_yield(csnip_coro_pth* C, void* val);
void* csnip_coro_pth_get_value(csnip_coro_pth* C);
void csnip_coro_pth_free(csnip_coro_pth* C);

#ifdef __cplusplus
}
#endif

#endif /* CSNIP_CORO_PTH_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_CORO_PTH_HAVE_SHORT_NAMES)
#define coro_pth_s		csnip_coro_pth_s
#define	coro_pth		csnip_coro_pth
#define coro_pth_func_ptr	csnip_coro_pth_func_ptr
#define coro_pth_new		csnip_coro_pth_new
#define coro_pth_set_func	csnip_coro_pth_set_func
#define coro_pth_next		csnip_coro_pth_next
#define coro_pth_yield		csnip_coro_pth_yield
#define coro_pth_get_value	csnip_coro_pth_get_value
#define coro_pth_free		csnip_coro_pth_free
#define CSNIP_CORO_PTH_HAVE_SHORT_NAMES
#endif

#endif /* CSNIP_HAVE_CORO_PTH */
