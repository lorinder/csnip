#ifndef CSNIP_LOG_H
#define CSNIP_LOG_H

/**	@file log.h
 *	@brief				Logging
 *	@defgroup log			Logging
 *	@{
 *
 *	@brief Logging utilities
 *
 *	This library provides a simple interface for logging.  Each log
 *	message is associated a priority as well as with context
 *	including the source file name and line number, but in
 *	particular a component name.  The component name is the
 *	expansion of the macro @a CSNIP_LOG_COMPONENT where
 *	csnip_log_Mesg() is expanded.
 *
 *	Log output can be filtered based on priority and regular
 *	expressions matching the component name, see csnip_log_config0()
 *	for details.
 */

#include <stdio.h>

/** Verbose debugging priority.
 *
 *  DEBUGV can be very detailed; possibly too detailed to run entire
 *  programs or larger modules with this debug level.
 */
#define CSNIP_LOG_PRIO_DEBUGV		0

/** Debugging priority.
 *
 *  These debug levels are suitable for general messages useful when
 *  debugging.
 */
#define CSNIP_LOG_PRIO_DEBUG		10

/** Informational priority. */
#define CSNIP_LOG_PRIO_INFO		20

/** Notice. */
#define CSNIP_LOG_PRIO_NOTICE		30

/** Warnings. */
#define CSNIP_LOG_PRIO_WARN		40

/** Errors. */
#define CSNIP_LOG_PRIO_ERR		50

#ifndef CSNIP_LOG_PRIO_MIN
/**	Minimum compiled-in prioriy.
 *
 *	This macro is only defined if not already predefined; the intent
 *	is the user can set it as desired.
 *
 *	Log messages with lower priorities will not be compiled in.
 *	Compiler optimizations, if enabled, will typically cause the
 *	string constants such as the components and file names, as well
 *	as the log message itself to be absent from binaries in those
 *	cases.
 */
#define CSNIP_LOG_PRIO_MIN		0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**	Simple logger configuration.
 *
 *	By default all messages of priority @a CSNIP_LOG_PRIO_NOTICE or
 *	higher will be logged.  This function provides a simple
 *	interface to configure the logger.
 *
 *	@param	filters_expr
 *		Filter expressions string.  The string is colon-separated
 *		string of expressions of the form RE~PRIO, where RE is a
 *		POSIX extended regular expression that is matched
 *		against the component string, and PRIO minimum priority
 *		value for a message to be printed.  If a message
 *		satisfies any of the filter expression constraints, it
 *		will be printed.  If RE is omitted, all messages are
 *		matched, if ~PRIO or PRIO is omitted, the priority is
 *		taken to be @a CSNIP_LOG_PRIO_NOTICE .  If @a
 *		filter_expr is NULL, messages of priorities at least the
 *		notice priority will be printed.
 *
 *	@param	log_out
 *		output file.
 */
void csnip_log_config0(const char* filters_expr,
		FILE* log_out);

/** @cond */
void csnip_log__mesg_trailer(FILE* fp);
void csnip_log__perror_trailer(FILE* fp);
void csnip_log__print(
		void (*print_trailer)(FILE*),
		int prio,
		const char* src_file,
		int src_line,
		const char* src_func,
		const char* component,
		const char* format,
		...);
/** @endcond */

#ifdef __cplusplus
}
#endif

#ifndef csnip_log_Mesg
/**	Log a message.
 *
 *	Logs a message;  this requires the @a CSNIP_LOG_COMPONENT macro
 *	to be defined.  The @a CSNIP_LOG_PRIO_MIN macro can be used to
 *	filter out lower priority messages at compile time for
 *	performance reasons.
 *
 *	See the documentation of csnip_log_config0() on what messages
 *	are printed by default and how the logger can be configured.
 *
 *	The @a CSNIP_LOG_COMPONENT must be a string with static
 *	lifetime, ideally a string literal.
 *
 *	@param	prio
 *		logging priority.
 *
 *	@param	...
 *		printf-style format and arguments.
 */
#define csnip_log_Mesg(prio, ...) \
	csnip_log_MesgForComp(CSNIP_LOG_COMPONENT, prio, __VA_ARGS__)
#endif

#ifndef csnip_log_MesgForComp
/**	Log a message for a specified component.
 *
 *	Variant of csnip_log_Mesg() with the logging component specified
 *	as an argument.  Does not require CSNIP_LOG_COMPONENT to be
 *	defined.
 *
 *	@param	comp
 *		the component, must be a string literal.
 *
 *	@param	prio
 *		logging priority.
 *
 *	@param	...
 *		printf-style format and arguments.
 */
#define csnip_log_MesgForComp(comp, prio, ...) \
	do { \
		if ((prio) >= CSNIP_LOG_PRIO_MIN) { \
			csnip_log__print( \
			  csnip_log__mesg_trailer, \
			  (prio), \
			  __FILE__, __LINE__, __func__, \
			  (comp), __VA_ARGS__); \
		} \
	} while (0)
#endif

#ifndef csnip_log_Perror
/**	Log an errno error.
 *
 *	This is the logging module's variant of perror(3).  It prints
 *	the log message as given here, and then, separated by a colon,
 *	the error message corresponding to the errno value.
 *
 *	@param	prio
 *		loggin priority.
 *
 *	@param	...
 *		printf-style format and arguments.
 */
#define csnip_log_Perror(prio, ...) \
	csnip_log_PerrorForComp(CSNIP_LOG_COMPONENT, prio, __VA_ARGS__)
#endif

#ifndef csnip_log
/**	Log an errno error for a specified component.
 *
 *	Variant of csnip_log_Perror() with the logging component
 *	specified as an argument.  Does not require CSNIP_LOG_COMPONENT
 *	to be defined.
 *
 *	@param	comp
 *		the component, must be a string literal.
 *
 *	@param	prio
 *		loggin priority.
 *
 *	@param	...
 *		printf-style format and arguments.
 */
#define csnip_log_PerrorForComp(comp, prio, ...) \
	do { \
		if ((prio) >= CSNIP_LOG_PRIO_MIN) { \
			csnip_log__print( \
			  csnip_log__perror_trailer, \
			  (prio), \
			  __FILE__, __LINE__, __func__, \
			  (comp), __VA_ARGS__); \
		} \
	} while (0)
#endif

/** @} */

#endif /* CSNIP_LOG_H */

/* Short names */
#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_LOG_HAVE_SHORT_NAMES)
#define LOG_PRIO_DEBUGV		CSNIP_LOG_PRIO_DEBUGV
#define LOG_PRIO_DEBUG		CSNIP_LOG_PRIO_DEBUG
#define LOG_PRIO_INFO		CSNIP_LOG_PRIO_INFO
#define LOG_PRIO_NOTICE		CSNIP_LOG_PRIO_NOTICE
#define LOG_PRIO_WARN		CSNIP_LOG_PRIO_WARN
#define LOG_PRIO_ERR		CSNIP_LOG_PRIO_ERR
#define log_config0		csnip_log_config0
#define log_Mesg		csnip_log_Mesg
#define log_MesgForComp		csnip_log_MesgForComp
#define log_Perror		csnip_log_Perror
#define log_PerrorForComp	csnip_log_PerrorForComp
#define CSNIP_LOG_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES */
