#ifndef CSNIP_X_H
#define CSNIP_X_H

/**	@file x.h
 *	@brief			Compatibility functions
 *	@defgroup		Compatibility functions
 *	@{
 *
 *	@brief Compatibility functions.
 *
 *	This module contains functions that can be found on some
 *	systems, but are missing from others.
 */

#include <stddef.h>

int csnip_x_strerror_r(int errnum, char* buf, size_t buflen);

/** @} */

#endif /* CSNIP_X_H */

#if defined(CSNIP_SHORT_NAMES) && !defined(CSNIP_X_HAVE_SHORT_NAMES)
#define x_strerror_r			csnip_x_strerror_r
#define CSNIP_X_HAVE_SHORT_NAMES
#endif /* CSNIP_SHORT_NAMES && !CSNIP_X_HAVE_SHORT_NAMES */
