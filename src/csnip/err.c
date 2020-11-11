#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <csnip/err.h>
#include <csnip/x.h>

/* Handles static error strings. */
static char* errstr_static(int errnum)
{
	switch(errnum) {
	case csnip_err_UNDERFLOW:	return "Underflow";
	case csnip_err_RANGE:		return "Value out of range";
	case csnip_err_FORMAT:		return "Wrong data format";
	case csnip_err_UNEXPECTED_NULL:	return "Unexpected NULL pointer";
	case csnip_err_INVAL:		return "Invalid argument";
	case csnip_err_CALLFLOW:	return "Wrong call flow";
	default:			return NULL;
	};
}

static bool errstr_dynamic(int errnum, char* buf, size_t buf_len)
{
	int errno_value = -1;

	switch(errnum) {
	case csnip_err_SUCCESS:		errno_value = 0;
	case csnip_err_ERRNO:		errno_value = errno;
	case csnip_err_NOMEM:		errno_value = ENOMEM;
	default:
		return false;
	};

	csnip_x_strerror_r(errno_value, buf, buf_len);
	return true;
}

void csnip_err_str(int errnum, char* buf, size_t buf_len)
{
	/* Static error strings */
	char* estr = errstr_static(errnum);
	if (estr) {
		snprintf(buf, buf_len, "%s", estr);
		return;
	}

	/* Dynamic error strings */
	if (errstr_dynamic(errnum, buf, buf_len))
		return;

	snprintf(buf, buf_len, "Unknown error");
}
