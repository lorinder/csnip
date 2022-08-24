#ifndef CSNIP_MEANVAR_H
#define CSNIP_MEANVAR_H

/**	@file	meanvar.h
 *	@brief			Mean and variance computation
 *	@defgroup meanvar	Mean and variance computation
 *	@{
 *
 *	Simple module to compute mean and variance of a sequence of
 *	values.  The implementation is based on Welford's online
 *	algorithm which, unlike the naive method, is numerically
 *	stable.  The module uses a constant amount of memory and is
 *	malloc()-free.
 *
 *	Three accumulator types and sets of functions are provided by
 *	default:
 *
 *	* csnip_meanvar, and the functions csnip_meanvar_add(),
 *	  csnip_meanvar_mean() and csnip_meanvar_var() are used to
 *	  accumulate samples of type double.
 *
 *	* csnip_meanvarf, and the functions named csnip_meanvarf_* are
 *	  used for float samples.
 *
 *	* csnip_meanvarl, and csnip_meanvarl_* functions, are used for
 *	  long double samples.
 *
 *	Further, the macros CSNIP_MEANVAR_DEF_TYPE(),
 *	CSNIP_MEANVAR_DECL_FUNCS() and CSNIP_MEANVAR_DEF_FUNCS() can be
 *	used to define accumulators for other data types.
 *
 *	A simple usage example is as follows:
 *
 *	\include meanvar.c
 */

#ifdef __cplusplus
extern "C" {
#endif

/**	Accumulator for sample data.
 *
 *	This structure keeps track of the necessary information to
 *	accumulate sample information.  To start, it should be zero
 *	initialized.
 *
 *	This version is for double precision, similar types for float
 *	and long double precision exist as well, called csnip_meanvarf
 *	and csnip_meanvarl, respectively.
 *
 *	Furthermore, accumulators can be defined for other types using
 *	the CSNIP_MEANVAR_DEF_TYPE() macro; that could be used, e.g., to
 *	define an accumulator for _Decimal64.
 */
typedef struct {
	long int count;		/**< Number of samples */
	double M;		/**< Running mean */
	double S;		/**< Running square difference */
} csnip_meanvar;

/**	Define an accumulator type.
 *
 *	@param	aggr_name
 *		Name of the accumulator type to define.
 *
 *	@param	val_type
 *		Value type to use.
 */
#define CSNIP_MEANVAR_DEF_TYPE(aggr_name, val_type) \
	typedef struct { \
		long int count; \
		val_type M; \
		val_type S; \
	} aggr_name;

CSNIP_MEANVAR_DEF_TYPE(csnip_meanvarf, float)
CSNIP_MEANVAR_DEF_TYPE(csnip_meanvarl, long double)

/**	Declare accumulator functions.
 *
 *	Declares functions to operate on an accumulator type; useful
 *	when custom accumulator types are defined.
 */
#define CSNIP_MEANVAR_DECL_FUNCS(scope, prefix, accum_type, val_type) \
	scope void prefix ## add(accum_type* A, val_type v); \
	scope val_type prefix ## mean(const accum_type* A); \
	scope val_type prefix ## var(const accum_type* A, val_type ddof);

/**	Add sample to accumulator.
 *
 *	@param	A
 *		Pointer to the accumulator.
 *
 *	@param	v
 *		The sample (value) to be added.
 */
void csnip_meanvar_add(csnip_meanvar* A, double v);

/**	Compute the mean.
 *
 *	Compute the mean of all samples currently added.
 *
 *	@param	A
 *		Pointer to the accumulator
 *
 *	@return Computed mean.
 */
double csnip_meanvar_mean(const csnip_meanvar* A);

/**	Compute the variance.
 *
 *	Compute the variance of the samples.
 *
 *	@param	A
 *		Pointer to the accumulator.
 *
 *	@param	ddof
 *		Compensation amount for degrees of freedom; use 0 for
 *		population variance and 1 for sample variance.
 *
 *	@return	Computed variance.
 */
double csnip_meanvar_var(const csnip_meanvar* A, double ddof);

CSNIP_MEANVAR_DECL_FUNCS(, csnip_meanvarf_, csnip_meanvarf, float)
CSNIP_MEANVAR_DECL_FUNCS(, csnip_meanvarl_, csnip_meanvarl, long double)

/**	Define accumulator functions.
 *
 *	Defines functions to operate on an accumulator type; this can be
 *	used for custom accumulator types defined with
 *	CSNIP_MEANVAR_DEF_TYPE().
 */
#define CSNIP_MEANVAR_DEF_FUNCS(scope, prefix, accum_type, val_type) \
	scope void prefix ## add(accum_type* A, val_type v) \
	{ \
		const val_type last_M = A->M; \
		++A->count; \
		A->M = last_M + (v - last_M) / A->count; \
		A->S = A->S + (v - last_M) * (v - A->M); \
	} \
	\
	scope val_type prefix ## mean(const accum_type* A) \
	{ \
		return A->M; \
	} \
	\
	scope val_type prefix ## var(const accum_type* A, \
					val_type ddof) \
	{ \
		return A->S / (A->count - ddof); \
	} \

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus

/**	Add a value to a meanvar accumulator.
 *
 *	@param	accumptr
 *		Pointer to the accumulator to which the value is added.
 *		Must be of type csnip_meanvar, csnip_minvarf or
 *		csnip_minvarl.
 *
 *	@param	value
 *		The value to be added.
 *
 *	Each macro parameter is evaluated exactly once.
 *
 *	*Requirements*:  C11's _Generic, or C++ templates.  The
 *	corresponding typed functions csnip_meanvar_add{f,,l} are not
 *	dependent on C11.
 */
#define csnip_meanvar_Add(accumptr, value) \
	_Generic(*(accumptr), \
	  csnip_meanvarf: \
	    csnip_meanvarf_add((csnip_meanvarf*)(accumptr), (value)), \
	  csnip_meanvar: \
	    csnip_meanvar_add((csnip_meanvar*)(accumptr), (value)), \
	  csnip_meanvarl: \
	    csnip_meanvarl_add((csnip_meanvarl*)(accumptr), (value)))

/**	Return the current mean.
 *
 *	@param	accumptr
 *		Pointer to the accumulator
 *
 *	*Requirements*: C11's _Generic or C++ templates.  The
 *	corresponding typed functions csnip_meanvar_mean{f,,l} are not
 *	dependent on C11.
 */
#define csnip_meanvar_Mean(accumptr) \
	_Generic(*(accumptr), \
	  csnip_meanvarf: \
	    csnip_meanvarf_mean((csnip_meanvarf*)(accumptr)), \
	  csnip_meanvar: \
	    csnip_meanvar_mean((csnip_meanvar*)(accumptr)), \
	  csnip_meanvarl: \
	    csnip_meanvarl_mean((csnip_meanvarl*)(accumptr)))

/**	Return the current variance.
 *
 *	@param	accumptr
 *		Pointer to the accumulator
 *
 *	@param	ddof
 *		"Delta degrees of freedom".  Set this to 1 if Bessel's
 *		correction is to be applied (divisor N - 1), or 0 if not
 *		(divisor N).  In general, the divisor will be N - ddof.
 *
 *	*Requirements*: C11's _Generic or C++ templates.  The
 *	corresponding typed functions csnip_meanvar_var{f,,l} are not
 *	dependent on C11.
 */
#define csnip_meanvar_Var(accumptr, ddof) \
	_Generic(*(accumptr), \
	  csnip_meanvarf: \
	    csnip_meanvarf_var((csnip_meanvarf*)(accumptr), (ddof)), \
	  csnip_meanvar: \
	    csnip_meanvar_var((csnip_meanvar*)(accumptr), (ddof)), \
	  csnip_meanvarl: \
	    csnip_meanvarl_var((csnip_meanvarl*)(accumptr), (ddof)))

#else /* __cplusplus */

/* C++ version of the generic csnip_meanvar_* API */

/**	Type information for an accumulator.
 *
 *	This template allows to determine the scalar type underlying a
 *	meanvar accumulator type.  Each instantiation has a typedef @a
 *	scalar to the scalar type.
 */
template<typename T> class csnip_meanvar_cxx_typeinfo;

#define CSNIP__SCALAR(accum_type) \
	typename csnip_meanvar_cxx_typeinfo<accum_type>::scalar

template<> struct csnip_meanvar_cxx_typeinfo<csnip_meanvarf> {
	typedef float scalar;
};

template<> struct csnip_meanvar_cxx_typeinfo<csnip_meanvar> {
	typedef double scalar;
};

template<> struct csnip_meanvar_cxx_typeinfo<csnip_meanvarl> {
	typedef long double scalar;
};

template<typename T> void csnip_meanvar__cxx_add(T*, CSNIP__SCALAR(T));

/* Instantiate the template for adding.
 *
 * The @a dummy argument here is to avoid warnings about missing macro
 * arguments on some compilers:  When the suffix is empty, it looks like
 * we're missing a macro argument when in actuality, we supply an empty
 * one.  To make this more explicit, the dummy parameter will make the
 * macro invocation look like this: MACRO(dummy,).  This makes it clear
 * for the reader and the compiler, that there are two arguments, the
 * second one of which is empty.
 */
#define CSNIP__DEF_ADD(dummy, suffix) \
	template<> void csnip_meanvar__cxx_add(csnip_meanvar##suffix* accum, \
			CSNIP__SCALAR(csnip_meanvar##suffix) val) \
	{ \
		csnip_meanvar##suffix##_add(accum, val); \
	}
CSNIP__DEF_ADD(dummy,f)
CSNIP__DEF_ADD(dummy,)
CSNIP__DEF_ADD(dummy,l)
#undef CSNIP__DEF_ADD

#define csnip_meanvar_Add(accum, val) csnip_meanvar__cxx_add(accum, val)

template<typename T> CSNIP__SCALAR(T) csnip_meanvar__cxx_mean(const T*);

#define CSNIP__DEF_MEAN(dummy,suffix) \
	template<> CSNIP__SCALAR(csnip_meanvar##suffix) \
		csnip_meanvar__cxx_mean(const csnip_meanvar##suffix* accum) \
	{ \
		return csnip_meanvar##suffix##_mean(accum); \
	}
CSNIP__DEF_MEAN(dummy,f)
CSNIP__DEF_MEAN(dummy,)
CSNIP__DEF_MEAN(dummy,l)
#undef CSNIP__DEF_MEAN

#define csnip_meanvar_Mean(accum) csnip_meanvar__cxx_mean(accum)

template<typename T> \
	CSNIP__SCALAR(T) csnip_meanvar__cxx_var(const T*, CSNIP__SCALAR(T));

#define CSNIP__DEF_VAR(dummy,suffix) \
	template<> CSNIP__SCALAR(csnip_meanvar##suffix) \
		csnip_meanvar__cxx_var(const csnip_meanvar##suffix* accum, \
			CSNIP__SCALAR(csnip_meanvar##suffix) ddof) \
	{ \
		return csnip_meanvar##suffix##_var(accum, ddof); \
	}
CSNIP__DEF_VAR(dummy,f)
CSNIP__DEF_VAR(dummy,)
CSNIP__DEF_VAR(dummy,l)
#undef CSNIP__DEF_VAR
#undef CSNIP__SCALAR

#define csnip_meanvar_Var(accum, ddof) csnip_meanvar__cxx_var(accum, ddof)

#endif /* __cplusplus */

/** @} */

#endif /* CSNIP_MEANVAR_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_MEANVAR_HAVE_SHORT_NAMES)
#define meanvarf		csnip_meanvarf
#define meanvar			csnip_meanvar
#define meanvarl		csnip_meanvarl
#define MEANVAR_DEF_TYPE	CSNIP_MEANVAR_DEF_TYPE
#define meanvarf_add		csnip_meanvarf_add
#define meanvar_add		csnip_meanvar_add
#define meanvarl_add		csnip_meanvarl_add
#define meanvarf_mean		csnip_meanvarf_mean
#define meanvar_mean		csnip_meanvar_mean
#define meanvarl_mean		csnip_meanvarl_mean
#define meanvarf_var		csnip_meanvarf_var
#define meanvar_var		csnip_meanvar_var
#define meanvarl_var		csnip_meanvarl_var
#define meanvar_Add		csnip_meanvar_Add
#define meanvar_Mean		csnip_meanvar_Mean
#define meanvar_Var		csnip_meanvar_Var
#define CSNIP_MEANVAR_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_MEANVAR_HAVE_SHORT_NAMES */
