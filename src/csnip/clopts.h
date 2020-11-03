#ifndef CSNIP_CLOPTS_H
#define CSNIP_CLOPTS_H

/** @file clopts.h
 *  @brief		Command line options parser
 *  @defgroup clopts	Command line options parser
 *  @{
 *
 *  Convenient Command line options parser.
 *
 *  This parser automates many of the tedious and repetitive tasks that
 *  arise when using getopts and libraries based on similar APIs:
 *
 *  1.  It supports assigning to variables.  Values are converted to the
 *      appropriate form (e.g. argument values mapped to integers are
 *      parsed as integers by the library).
 *
 *  2.  It supports documentation of the command line flags and
 *      has a premade implementation of the --help flag.
 *
 *  3.  Errors are automatically handled.
 *
 *  Generally, clopts-based parsers are much simpler and shorter to
 *  write than getopt() based ones.  An example program using the main
 *  features of clopts is examples/clopts.c.
 *
 *  An example on how to perform command line option parsing with the
 *  clopts library is as follows:
 *
 *  \include clopts.c
 *
 *  This example can be found in examples/clopts.c.  For contrast, that
 *  same directory also contains examples/getopt.c which implements a
 *  similar program using the familiar getopt interface that POSIX
 *  provides.  Users familiar with getopt() should be able to get the
 *  hang of clopts fairly easily by studying these examples.  Note that
 *  the getopt.c example, despite missing quite a few things such as
 *  input validation on the numbers provided, or long options, is far
 *  longer than the clopts example.
 */

#include <stdio.h>
#include <stdbool.h>

#include <csnip/err.h>

#ifdef __cplusplus
extern "C" {
#endif

struct csnip_clopts_optinfo_s;
struct csnip_clopts_s;

/** Short typedef for struct csnip_clopts_optinfo_s */
typedef struct csnip_clopts_optinfo_s csnip_clopts_optinfo;

/** Short typedef for struct csnip_clopts_s. */
typedef struct csnip_clopts_s csnip_clopts;

/** Argument parser function pointer type */
typedef int (*csnip_clopts_parser)(const csnip_clopts* opts,
					const csnip_clopts_optinfo* optinfo,
					const char* argval);

/** Descriptor for a single command line option. */
struct csnip_clopts_optinfo_s {
	char short_name;		/**< Short (single character) form */
	const char* long_name;		/**< Long option form */
	const char* description;	/**< Option description */

	bool takes_val;			/**< True if option takes a value,
					     false if it's a flag */
	csnip_clopts_parser parser;	/**< The parser function callback */

	void* usr;			/**< User data pointer for the
					     callback */
};

/** Set of descriptors for all command line options. */
struct csnip_clopts_s {
	/* The array of clopts_optinfo members */
	int n_optinfo;			/**< Number of command line options */
	int n_optinfo_cap;		/**< Capacity of the option array */
	csnip_clopts_optinfo* optinfo;	/**< The options array */

	/** General program description.
	 *
	 *  This description is used typically to describe the purpose
	 *  of a program; it is output before the individual option
	 *  descriptions.
	 */
	const char *description;
};

/* Operations on csnip_clopts */

/** Add handlers for default options.
 *
 *  This adds in particular a useful handler for the -h / --help command
 *  line flag.
 */
int csnip_clopts_add_defaults(csnip_clopts* opts);

/** Add a processable option. */
int csnip_clopts_add(csnip_clopts* opts,
			int n,
			const csnip_clopts_optinfo* args_to_add);

/** Process command line arguments.
 *
 *  The command line options passed in the vector (argc, argv) are
 *  processed.  The program name argument argv[0] that is typically
 *  passed to main() is assumed not to be present, therefore an offset
 *  to the main args should be applied.
 *
 *  @return	0	on success
 *		< 0	csnip error code.  In the typical case of an
 *			error in the format sequence, an error message
 *			will have been printed to stderr.
 */
int csnip_clopts_process(csnip_clopts* opts,
			int argc,
			char** argv,
			int* ret_pos_args);

/** @defgroup CloptsParsers	Argument parsers for clopts
 *  @{
 */

/** Parse argument as integer of type char. */
int csnip_clopts_parser_uchar(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as an int. */
int csnip_clopts_parser_uint(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as a long int. */
int csnip_clopts_parser_ulong(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as long long int. */
int csnip_clopts_parser_ullong(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as float. */
int csnip_clopts_parser_float(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as double. */
int csnip_clopts_parser_double(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Parse argument as long double. */
int csnip_clopts_parser_ldouble(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);
/** Assign a char* to point to the argument.
 *
 *  This parser is typically used for string arguments.  Note that the
 *  string is not copied;  the user needs to take care that it is not
 *  destroyed.  In the typical case where it points to an argv-element,
 *  no special care needs to be taken since the argments are allocated
 *  for the lifetime of the program.
 */
int csnip_clopts_parser_pchar(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);

/** Parser for flags, bool target */
int csnip_clopts_flagparser_bool(const csnip_clopts* opts,
			const csnip_clopts_optinfo* optinfo,
			const char* argval);

/** @} */

#ifdef __cplusplus
}
#endif

/**  Add an option, generic variant.
 *
 *   This macro adds an option to the array of options, deducing the
 *   parser to use from the type of the variable to be assigned.
 *
 *   @param[in,out]	opts
 *			the csnip_clopts to add the option to
 *
 *   @param[in]		short_name
 *			Short-form name of the option.  This is a char.
 *
 *   @param[in]		long_name
 *			Long-form name of the option.  This is a C
 *			string.  The passed string is not copied and
 *			must therefore exist for the lifetime of the
 *			call.  Use NULL if no long name should be set.
 *
 *   @param[in]		description
 *			Description of the option.  This is shown to the
 *			user when --help is used, for example.
 *
 *   @param[in]		ptr_target
 *			The address of the target value.  This is where
 *			the parser option value will be placed.
 *
 *   @param[in,out]	err
 *			l-value for error return.
 */
#define csnip_clopts_Addvar(opts, \
			short_name, \
			long_name, \
			description, \
			ptr_target, \
			err) \
	csnip_clopts__Addvar((opts), (short_name), (long_name), \
	                (description), (ptr_target), (err), \
			csnip__parser, csnip__O, csnip__errval)

/** @cond */
#define csnip_clopts__Addvar(opts_, \
			short_name_, \
			long_name_, \
			description_, \
			ptr_target_, \
			err, \
			/* Internal variable names: */ \
			parser_, \
			O, \
			errval) \
	do { \
		csnip_clopts_parser parser_ = csnip_clopts__Getparser(ptr_target_); \
		csnip_clopts_optinfo O = { \
			.short_name = short_name_, \
			.long_name = long_name_, \
			.description = description_, \
			.takes_val = true, \
			.parser = parser_, \
			.usr = (void*)ptr_target_, \
		}; \
		int errval = csnip_clopts_add(opts_, 1, &O); \
		if (errval) \
			csnip_err_Raise(errval, err); \
	} while(0)
/** @endcond */

/**  Add a flag, generic variant.
 *
 *   This macro adds a flag (i.e., an option without value) to the array
 *   of options, deducing the parser to use from the type of the
 *   variable to be assigned.
 *
 *   @param[in,out]	opts
 *			the csnip_clopts to add the option to
 *
 *   @param[in]		short_name
 *			Short-form name of the flag.  This is a char.
 *
 *   @param[in]		long_name
 *			Long-form name of the flag.  This is a C
 *			string.  The passed string is not copied and
 *			must therefore exist for the lifetime of the
 *			call.  Use NULL if no long name should be set.
 *
 *   @param[in]		description
 *			Description of the option.  This is shown to the
 *			user when --help is used, for example.
 *
 *   @param[in]		ptr_target
 *			The address of the target value.  This is where
 *			the parser option value will be placed.
 *
 *   @param[in,out]	err
 *			l-value for error return.
 */
#define csnip_clopts_Addflag(opts, \
			short_name, \
			long_name, \
			description, \
			ptr_target, \
			err) \
	csnip_clopts__Addflag((opts), (short_name), (long_name), \
	                (description), (ptr_target), (err), \
			csnip__O, csnip__errval)

/** @cond */
#define csnip_clopts__Addflag(opts_, \
			short_name_, \
			long_name_, \
			description_, \
			ptr_target_, \
			err, \
			O, \
			errval) \
	do { \
		csnip_clopts_optinfo O = { \
			.short_name = short_name_, \
			.long_name = long_name_, \
			.description = description_, \
			.takes_val = false, \
			.parser = csnip_clopts_flagparser_bool, \
			.usr = (void*)ptr_target_ \
		}; \
		int errval = csnip_clopts_add(opts_, 1, &O); \
		if (errval) \
			csnip_err_Raise(errval, err); \
	} while (0)
/** @endcond */


#ifndef __cplusplus

/* C11 version using _Generic */
/** @cond */
#define csnip_clopts__Getparser(ptr_target_) \
		  _Generic(*ptr_target_, \
		    char: csnip_clopts_parser_uchar, \
		    unsigned char: csnip_clopts_parser_uchar, \
		    int: csnip_clopts_parser_uint, \
		    unsigned int: csnip_clopts_parser_uint, \
		    long: csnip_clopts_parser_ulong, \
		    unsigned long: csnip_clopts_parser_ulong, \
		    long long: csnip_clopts_parser_ullong, \
		    unsigned long long: csnip_clopts_parser_ullong, \
		    float: csnip_clopts_parser_float, \
		    double: csnip_clopts_parser_double, \
		    long double: csnip_clopts_parser_ldouble, \
		    char*: csnip_clopts_parser_pchar, \
		    char const*: csnip_clopts_parser_pchar)
/** @endcond */
#else

/* C++ version using templates */
/** @cond */
template<typename T> csnip_clopts_parser csnip_clopts__getparser(T* type);
#define csnip_clopts__tspec(type, suffix) \
	template<> csnip_clopts_parser csnip_clopts__getparser<type>(type*) \
	{ \
		return csnip_clopts_parser_ ## suffix; \
	}
csnip_clopts__tspec(char, uchar)
csnip_clopts__tspec(unsigned char, uchar)
csnip_clopts__tspec(int, uint)
csnip_clopts__tspec(unsigned int, uint)
csnip_clopts__tspec(long, ulong)
csnip_clopts__tspec(unsigned long, ulong)
csnip_clopts__tspec(long long, ullong)
csnip_clopts__tspec(unsigned long long, ullong)
csnip_clopts__tspec(float, float)
csnip_clopts__tspec(double, double)
csnip_clopts__tspec(long double, ldouble)
csnip_clopts__tspec(char*, pchar)
csnip_clopts__tspec(char const*, pchar)
#undef csnip_clopts__tspec
#define csnip_clopts__Getparser(ptr_target_) csnip_clopts__getparser(ptr_target_)
/** @endcond */

#endif

#ifdef CSNIP_SHORT_NAMES
#define clopts			csnip_clopts
#define clopts_optinfo		csnip_clopts_optinfo
#define clopts_parser		csnip_clopts_parser
#define clopts_init		csnip_clopts_init
#define clopts_clear		csnip_clopts_clear
#define clopts_add_defaults	csnip_clopts_add_defaults
#define clopts_add		csnip_clopts_add
#define clopts_process		csnip_clopts_process
#define clopts_parser_uchar	csnip_clopts_parser_uchar
#define clopts_parser_uint	csnip_clopts_parser_uint
#define clopts_parser_ulong	csnip_clopts_parser_ulong
#define clopts_parser_ullong	csnip_clopts_parser_ullong
#define clopts_parser_float	csnip_clopts_parser_float
#define clopts_parser_double	csnip_clopts_parser_double
#define clopts_parser_ldouble	csnip_clopts_parser_ldouble
#define clopts_parser_pchar	csnip_clopts_parser_pchar
#define clopts_Addvar		csnip_clopts_Addvar
#define clopts_Addflag		csnip_clopts_Addflag
#endif

/** @}*/

#endif /* CSNIP_CLOPTS_H */