#include <errno.h>
#include <string.h>

#include <csnip/err.h>

// XXX: Need to make this portable and thread safe
const char* csnip_err_str(int csnip_errnumber)
{
	switch(csnip_errnumber) {
	case csnip_err_SUCCESS:		return strerror(0);
	case csnip_err_ERRNO:		return strerror(errno); // XXX: race
	case csnip_err_NOMEM:		return strerror(ENOMEM);
	case csnip_err_UNDERFLOW:	return "Underflow";
	case csnip_err_RANGE:		return "Value out of range";
	case csnip_err_FORMAT:		return "Wrong data format";
	case csnip_err_UNEXPECTED_NULL:	return "Unexpected NULL pointer";
	default:			return "Unknown error";
	};
}
