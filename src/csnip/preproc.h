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
/** @endcond */

/** @} */

#endif /* CSNIP_PREPROC_H */
