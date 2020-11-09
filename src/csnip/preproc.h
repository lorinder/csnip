#ifndef CSNIP_PREPROC_H
#define CSNIP_PREPROC_H

/**	@file preproc.h
 *	@defgroup pp	Preprocessor macros
 *	@{
 *
 *	Preprocessor macros.
 *
 *	A number of somewhat tricky preprocessor macros.
 */

/** Prepend argument list to another. */
#define csnip_pp_prepend_args(...)		__VA_ARGS__,

/** Append argument list to another. */
#define csnip_pp_append_args(...)		,__VA_ARGS__

/** List arguments. */
#define csnip_pp_list_args(...)			__VA_ARGS__

/** Prepend an empty list to another list. */
#define csnip_pp_prepend_noargs()

/** Append an empty list to another list. */
#define csnip_pp_append_noargs()

/** List an empty argument list. */
#define csnip_pp_list_noargs()			void

/** Concatenate two tokens after macro substitution.
 *
 *  This macro concatenates @a a and @a b, first substituting them as
 *  macros if applicable.
 */
#define csnip_pp_cat(a, b) csnip_pp__cat(a, b)
/** @cond */
#define csnip_pp__cat(a, b)	a ## b
/** @endcond
    @} */

#endif /* CSNIP_PREPROC_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_PREPROC_HAVE_SHORT_NAMES)
#define	pp_prepend_args		csnip_pp_prepend_args
#define pp_append_args		csnip_pp_append_args
#define pp_list_args		csnip_pp_list_args
#define pp_prepend_noargs	csnip_pp_prepend_noargs
#define pp_append_noargs	csnip_pp_append_noargs
#define pp_list_noargs		csnip_pp_list_noargs
#define pp_cat			csnip_pp_cat
#define CSNIP_PREPROC_HAVE_SHORT_NAMES
#endif
