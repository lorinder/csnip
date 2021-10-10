#ifndef CSNIP_FMT_H
#define CSNIP_FMT_H

/**	@file fmt.h
 *	@brief		String formatting / interpolation
 *	@defgroup fmt	String formatting / interpolation
 *	@{
 *
 *	@brief String formatting / interpolation toolbox.
 *
 *	The fmt module provides macros that can be used for string
 *	formatting.  The module is organized as a sort of toolbox,
 *	allowing the programmer a lot of flexibility in the process.  In
 *	particular, the way keys are mapped to values is freely
 *	programmable.  At the same time, care was taken to keep simple
 *	use cases easy to implement.
 *
 *	The main macros are csnip_fmt_Str() for normal C strings and the
 *	slightly more general csnip_fmt_Gen(), which also works for
 *	non-char strings and can be used without memory allocations, if
 *	desired.
 *
 *	A very simple example of a formatter is as follows:
 *
 *	\include fmt.c
 *
 *	The source code can be found in examples/fmt.c.
 */

#include <string.h>
#include <ctype.h>

#include <csnip/arr.h>
#include <csnip/err.h>

/**	Format / interpolate a string.
 *
 *	@param		fmtstr
 *			Format string.
 *
 *	@param		key_start
 *			Dummy variable standing for the beginning of a
 *			key.  Used as an input and potential output for
 *			the key scanner @a scan_key.  Also used to
 *			denote the start of a key by the key matcher
 *			@a get_val.
 *
 *	@param		key_end
 *			Dummy variable, denoting the key end.  Used as
 *			an output computed by the key scanner
 *			@a scan_key.
 *
 *	@param		val
 *			Dummy variable, denoting the value of a key.
 *			Used as an output variable of the @a get_val
 *			statement.
 *
 *	@param		next
 *			Dummy variable, used to denote the remainder of
 *			the format string after a key.  Used as an
 *			output variable to the key scanner @a scan_key.
 *
 *	@param		scan_key
 *			Statement to scan a key.  Given a pointer
 *			key_start to the beginning of a key, this
 *			statement sets @a key_end, a pointer just beyond
 *			the last part of the key, and @a next, a pointer
 *			to the first subsequent literal part of the
 *			format string.  It is allowable for the @a
 *			scan_key statement to also modify key_start.
 *			This can be useful e.g. to strip out nuisance
 *			characters at the beginning of a key.
 *
 *	@param		get_val
 *			statement to assign the value corresponding to
 *			the key delimited by @a key_start ... @a key_end-1 to
 *			the dummy variable val.
 *
 *	@param		free_val
 *			statement to release a used value. It
 *			can be used, e.g., when get_val dynamically
 *			generates the values and allocates memory for
 *			that purpose.  The free_val statement can then
 *			be used to free that key.
 *
 *	@param[out]	out
 *			Target variable to hold the output string, of type
 *			const char*.
 *
 *	@param[out]	err
 *			Error return.
 */
#define csnip_fmt_Str(fmtstr, \
			keychar, \
			key_start, \
			key_end, \
			val, \
			p, \
			scan_key, \
			get_val, \
			free_val, \
			out, \
			err) \
	do { \
		int len = 0; \
		int cap = 0; \
		out = NULL; \
		\
		csnip_fmt_Gen(char, \
			(fmtstr), \
			(keychar), \
			key_start, \
			key_end, \
			val, \
			p, \
			csnip_arr_Push(out, len, cap, *p, err), \
			scan_key, \
			get_val, \
			free_val, \
			err); \
	} while(0)

/**	Generalized formatter.
 *
 *	@param		ctype
 *			type of a character, e.g. "char" or "wchar_t".
 *
 *	@param		fmtstr
 *			Format string.
 *
 *	@param		key_start
 *			Dummy variable of type ctype*, referring to the
 *			beginning of a key.
 *
 *	@param		key_end
 *			Dummy variable of type ctype*, referring to the
 *			end of a key.
 *
 *	@param		val
 *			Dummy variable of type ctype*, and a NUL-terminated
 *			ctype* string.  Referring to the value of a key,
 *			as computed by @a get_val.
 *
 *	@param		p
 *			Dummy variable of type ctype*, pointing to a
 *			character of interest.  This variable has a dual
 *			use:  First, the @a scan_key statement assigns
 *			it the position after the key.   Second, the
 *			@a push_char statement uses it as an input.
 *
 *	@param		push_char
 *			statement that appends the character *p to
 *			the output string being constructed.
 *
 *	@param		scan_key
 *			this statement scans the key starting at @a
 *			key_start, which, initially points to the
 *			character immediately after the escape character.
 *			It assigns @a key_end to the character just past
 *			the key, and potentially adjusts @a key_start,
 *			so that the string @a key_start, ..., @a key_end
 *			- 1 contains the key.  It also assigns @a p to
 *			the first position after the key string.
 *
 *	@param		get_val
 *			statement assigning the value corresponding to
 *			the key @a key_start ... @a key_end - 1 to the
 *			variable var.
 *
 *	@param		free_val
 *			free value currently assigned to val; useful if
 *			val was dynamically allocated.
 *
 *	@param		err
 *			Error return.
 *
 */
#define csnip_fmt_Gen(ctype, \
			fmtstr, \
			keychar, \
			key_start, \
			key_end, \
			val, \
			p, \
			push_char, \
			scan_key, \
			get_val, \
			free_val, \
			err) \
	do { \
		const ctype* p = fmtstr; \
	\
		while (1) { \
			/* Scan the next literal */ \
			while (*p != '\0') { \
				if (*p == keychar) { \
					++p; \
					if (*p == '\0' || *p != keychar) \
						break; \
				} \
				do { push_char; } while(0); \
				++p; \
			} \
			if (*p == '\0') { \
				do { push_char; } while(0); \
				break; \
			} \
	\
			/* Find the end of the variable name */ \
			const ctype* key_start = p; \
			const ctype* key_end = NULL; \
			do { scan_key; } while(0); \
	\
			/* Look up value */ \
			const ctype* val = NULL; \
			if (key_end) { \
				do { get_val; } while (0); \
			} else { \
				/* Error: Key parsing error */ \
				csnip_err_Raise(csnip_err_FORMAT, err); \
				++p; \
			} \
	\
			/* Insert replacement string */ \
			if (val) { \
				const ctype* vp = p; \
				p = val; \
				while (*p) { \
					do { push_char; } while(0); \
					++p; \
				} \
				p = vp; \
				do { free_val; } while (0); \
			} else { \
				/* Error: Key not found */ \
				csnip_err_Raise(csnip_err_FORMAT, err); \
			} \
		} \
	} while (0)

/**	Key scanner for keys ending in a magic character.
 *
 *	This statement macro can be used to construct a key scanner for
 *	csnip_fmt_Str().  It finds the end of a key by looking for a
 *	given key character, i.e., starting from `*key_start`, it finds
 *	the next occurrence of `keychar`, then sets `key_end` to that
 *	position, and `next` to the subsequent position.
 *
 *	@param		key_start
 *			Variable. Corresponding to the dummy variable of
 *			csnip_fmt_Str() of the same name.  This macro
 *			reads, but does not write key_start.
 *
 *	@param		key_end
 *			Variable.  This macro sets that variable to the
 *			first found occurrence of keychar.
 *
 *	@param		next
 *			Variable.  The macro sets next to point to the
 *			first character after the key.
 *
 *	@param		keychar
 *			The separating character used to delimit the end
 *			of the key.
 */
#define csnip_fmt_ScanToChar(key_start, key_end, next, keychar) \
	do { \
		if ((key_end = strchr(key_start, keychar)) != NULL) \
			next = key_end + 1; \
	} while (0)

/**	Key scanner of sh-style keys.
 *
 *	This scans keys of the form `${xxx}` or `$xxx`, the latter being
 *	delimited at the end by a non-alphanumeric character distinct
 *	from `_`.  (Strictly speaking, keys don't need to start with
 *	`$`; the key character in csnip_fmt_Str() determines the
 *	beginning.)
 *
 *	@param[in/out]	key_start
 *			variable pointing initially to the character
 *			past the initial key character.  At the end of
 *			the call, will point to the start of the key
 *			name (e.g., `xxx` for the pattern `$xxx` or
 *			`${xxx}`).
 *
 *	@param[out]	key_end
 *			output variable pointing to the character past
 *			the last character of the key end.  Thus, in the
 *			case where the key is enclosed in curly braces.
 *			this will point to the ending `}`.  If there is
 *			a key parsing error, this will be set to NULL
 *			instead.
 *
 *	@param[out]	next
 *			output variable pointing to the first character
 *			past the end of the key expression after the
 *			call.
 */
#define csnip_fmt_ScanShStyle(key_start, key_end, next) \
	do { \
		if (*key_start == '{') { \
			++key_start; \
			key_end = key_start; \
			while (*key_end && *key_end != '}') \
				++key_end; \
			if (*key_end) \
				next = key_end + 1; \
			else \
				key_end = NULL; \
		} else { \
			key_end = key_start; \
			while (*key_end \
				&& (isalnum(*key_end) || *key_end == '_')) \
			{ \
				++key_end; \
			} \
			next = key_end; \
		} \
	} while (0)

/**	Key matcher for explicitly listed keys and values.
 *
 *	@param[in]	key_start
 *			Pointer to the beginning of the key.
 *
 *	@param[in]	key_end
 *			Pointer to the character past the key.
 *
 *	@param[out]	val
 *			Output value, will point to the value of the
 *			given key if found.  If not found, it remains
 *			unchanged.
 *
 *	@param[in]	...
 *			A sequence of keys and their values in
 *			alternating order, i.e., key1, val1,
 *			key2, val2, etc.
 */
#define csnip_fmt_ListMatch(key_start, key_end, val, keyval_pairs) \
	do { \
		const size_t len = key_end - key_start; \
		for (int i = 0; keyval_pairs[i]; i += 2) { \
			if (strlen(keyval_pairs[i]) == len \
			    && strncmp(keyval_pairs[i], key_start, len) == 0) \
			{ \
				val = keyval_pairs[i + 1]; \
				break; \
			} \
		} \
	} while (0)

#endif /* CSNIP_FMT_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_FMT_HAVE_SHORT_NAMES)
#define fmt_Str			csnip_fmt_Str
#define fmt_Gen			csnip_fmt_Gen
#define fmt_ScanToChar		csnip_fmt_ScanToChar
#define fmt_ScanShStyle		csnip_fmt_ScanShStyle
#define fmt_ListMatch		csnip_fmt_ListMatch
#define CSNIP_FMT_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_FMT_HAVE_SHORT_NAMES */
