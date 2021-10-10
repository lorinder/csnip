/**	@file toy_printf.c
 *
 *	@brief Toy printf() to illustrate fmt.
 *
 *	This example contains a very limited implementation of printf()
 *	based on csnip's fmt module.  Only %s, %d and %c are supported.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define CSNIP_SHORT_NAMES
#include <csnip/fmt.h>

/* size of static buffer for intermediate string.  must have enough
 * space to store all the digits of an int + '\0' + one more character.
 */
#define BUFSZ		30

static const char* scan_key(const char* key_start, const char** next)
{
	switch (*key_start) {
	case 'd':
	case 'c':
	case 's':
		*next = key_start + 1;
		return *next;
	default:
		return NULL;
	};
}

static const char* get_val(va_list* ap,
			char* buf,
			const char* key_start,
			const char* key_end)
{
	switch (*key_start) {
	case 'd': {
		/* Primitive way to convert int to string, works only
		 * for nonnegative values
		 */
		int j = va_arg(*ap, int);

		buf[BUFSZ - 1] = '\0';
		char* p = &buf[BUFSZ - 1];
		do {
			*p = '0' + (j % 10);
			j /= 10;
			--p;
		} while (j != 0 && p > buf);
		return p + 1;
	}
	case 'c':
		*buf = va_arg(*ap, int);
		buf[1] = '\0';
		return buf;
	case 's':
		return va_arg(*ap, const char*);
	};

	return NULL;
}

int toy_printf(const char* format, ...)
{
	char buf[BUFSZ];
	va_list ap;
	va_start(ap, format);

	int err = 0;
	int len = 0;
	csnip_fmt_Gen(char,					/* type */
		format,						/* format */
		'%',						/* key char */
		key_start, key_end, val, pos,			/* vars */
		(++len, putc(*pos, stdout)),			/* push */
		key_end = scan_key(key_start, &pos),		/* scan key */
		val = get_val(&ap, buf, key_start, key_end),	/* get val */
		/* no op */,					/* free val */
		err);						/* error */

	va_end(ap);
	return (err ? -1 : len);
}

int main(int argc, char** argv)
{
	toy_printf("%s is %d years old\n", "Jonathan", 312);
	toy_printf("%c%c%c%c%c\n", 'H', 'e', 'l', 'l', 'o');
	toy_printf("100%% certain!\n");

	return 0;
}
