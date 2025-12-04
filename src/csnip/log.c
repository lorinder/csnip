#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <csnip/csnip_conf.h>
#ifdef CSNIP_CONF__HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef CSNIP_CONF__HAVE_REGCOMP
#include <regex.h>
#endif
#ifdef CSNIP_CONF__SUPPORT_THREADING
#include <pthread.h>
#endif

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/fmt.h>
#include <csnip/list.h>
#include <csnip/log.h>
#include <csnip/lphash_table.h>
#include <csnip/mem.h>
#include <csnip/time.h>
#include <csnip/x.h>

/** Default logging priority */
#define PRIO_DEFAULT	CSNIP_LOG_PRIO_NOTICE

/**	Log filtering rule.
 *
 *	Each rule asserts that if the regular expression @a re matches
 *	the component of a log message, then it will be processed if its
 *	priority is at least equal to the @a prio field given.
 */
struct filter_rule_S {
#ifdef CSNIP_CONF__HAVE_REGCOMP
	regex_t re;		/**< Component matching expr */
#else
	char* substr;		/**< Substring to match */
#endif
	int prio;		/**< Minimum processing prio */

	struct filter_rule_S* next; /**!< Next in list */
};

/**	Priority cache entry.
 *
 *	The priority cache stores component name pointers and their
 *	associated minimum priority, as determined by previous filter
 *	lookups.
 */
typedef struct {
	const char* component;	/**< Cached pointer */
	int min_prio;		/**< Cached min prio value */
} comp_prio;

/**	Priority cache table. */
CSNIP_LPHASH_TABLE_DEF_TYPE(priotbl, comp_prio)

/**	Log Processor */
typedef struct {

	/** @{ Linked list of filter rules */
	struct filter_rule_S* rules_head;
	struct filter_rule_S* rules_tail;
	/** @} */

	/** Global minimum priority */
	int min_prio;

	/** Access lock.
	 *
	 *  This access lock protects access to ptbl.
	 */
#ifdef CSNIP_CONF__SUPPORT_THREADING
	pthread_rwlock_t lock;
#endif

	/** Priorities cache */
	struct priotbl* ptbl;

	/** Log format strings, one per style.
	 *
	 *  Styles:
	 *  0	is the default style
	 *  1	is for a perror-style message.
	 */
	char* logfmt[2];

	/** Logger output file */
	FILE* fp;

} csnip_log_processor;

static csnip_log_processor* proc = NULL;

CSNIP_LPHASH_TABLE_DEF_FUNCS(static cext_unused,	/* scope */
			ptbl_,				/* prefix */
			const char*,			/* key type */
			comp_prio,			/* entrytype */
			struct priotbl,			/* tbltype */
			k1, k2, e,			/* dummy vars */
			(size_t)k1,			/* hash(k1) */
			strcmp(k1, k2) == 0,		/* is_match(k1, k2) */
			(e).component)			/* get_key(e) */

/* csnip_log_processor methods */

static void proc_init(csnip_log_processor* P)
{
	P->rules_head = P->rules_tail = NULL;
	P->min_prio = 100;
	int err = 0;
#ifdef CSNIP_CONF__SUPPORT_THREADING
	pthread_rwlock_init(&P->lock, NULL);
#endif
	P->ptbl = ptbl_make(&err); // error handling
	for (int i = 0; i < Static_len(P->logfmt); ++i)
		P->logfmt[i] = NULL;
	P->fp = NULL;
}

static void proc_free(csnip_log_processor* P)
{
	/* Free all the filter rules */
	struct filter_rule_S* next;
	for (struct filter_rule_S* h = P->rules_head;
		h != NULL;
		h = next)
	{
#ifdef CSNIP_CONF__HAVE_REGCOMP
		regfree(&h->re);
#else
		mem_Free(h->substr);
#endif
		next = h->next;
		mem_Free(h);
	}

	/* Free lock */
#ifdef CSNIP_CONF__SUPPORT_THREADING
	pthread_rwlock_destroy(&P->lock);
#endif

	/* Free the hashing table */
	ptbl_free(P->ptbl);

	/* Free */
	mem_Free(P);
}

static void proc_add_filters(csnip_log_processor* P, char* filters_expr)
{
	char* sptr = NULL;
	char* expr;
	while ((expr = x_strtok_r(filters_expr, ":", &sptr)) != NULL) {
		/* Clear filters_expr for subsequent strtok_r */
		filters_expr = NULL;

		/* Split off priority */
		int prio = PRIO_DEFAULT;
		char* p = strrchr(expr, '~');
		if (p) {
			*p++ = '\0';
			if (*p) {
				prio = atoi(p);
			}
		}

		/* Process expression */
		if (*expr) {
			/* Non-empty expression:  Create new filter_rule_S */
			struct filter_rule_S* ent;
			mem_Alloc(1, ent, _);
			ent->prio = prio;
#ifdef CSNIP_CONF__HAVE_REGCOMP
			int err = regcomp(&ent->re,
					expr,
					REG_EXTENDED|REG_NOSUB);
#else
			ent->substr = x_strdup(expr);
			int err = (ent->substr == NULL);
#endif
			if (err == 0) {
				slist_PushTail(P->rules_head,// head
						P->rules_tail,	// tail
						next,		// mnext
						ent);		// el
			} else {
#ifdef CSNIP_CONF__HAVE_REGCOMP
				char errbuf[80];
				regerror(err, &ent->re, errbuf, sizeof(errbuf));
				fprintf(stderr,
				  "Error:  regex `%s': %s\n",
				  expr, errbuf);
				mem_Free(ent);
#else
				fprintf(stderr, "Error:  Out of memory\n");
#endif
			}
		} else {
			/* Empty regex:  Adjust global priority */
			if (prio < P->min_prio) {
				P->min_prio = prio;
			}
		}
	}
}

/* ... */

int csnip_log_config0(const char* filter_expr,
		FILE* out_fp)
{
	csnip_log_configuration cfg = {
		.filter_expr = filter_expr,
		.out_fp = out_fp,
	};
	return csnip_log_config(&cfg);
}

int csnip_log_config(const csnip_log_configuration* cfg)
{
	/* Clear out old, if any */
	if (proc) {
		proc_free(proc);
		proc = NULL;
	}

	mem_Alloc(1, proc, _);
	proc_init(proc);

	/* Create the `re` array */
	if (cfg->filter_expr != NULL) {
		proc->min_prio = 100;

		char* fe2 = x_strdup(cfg->filter_expr);
		proc_add_filters(proc, fe2);
		free(fe2);
	} else {
		proc->min_prio = PRIO_DEFAULT;
	}

	/* copy the log formats */
	for (int i = 0; i < Static_len(cfg->logfmt); ++i) {
		if (cfg->logfmt[i] == NULL) {
			proc->logfmt[i] = NULL;
		} else {
			proc->logfmt[i] = x_strdup(cfg->logfmt[i]);
		}
	}

	/* Set the log file target */
	proc->fp = (cfg->out_fp ? cfg->out_fp : stderr);

	return 0;
}

void csnip_log_free(void)
{
	if (proc == NULL)
		return;
	for (int i = 0; i < Static_len(proc->logfmt); ++i) {
		if (proc->logfmt[i]) {
			free(proc->logfmt[i]);
		}
	}

	proc_free(proc);
	proc = NULL;
}

extern inline const char* csnip_log__file(const char* filepath);

typedef enum {
	TS_UTC,
	TS_LOCAL,
	TS_MONO,
} TsType;

static const char* put_timestamp(char* buf, size_t bufSz, TsType tsType)
{
	struct timespec ts;
	csnip_x_clock_gettime(CLOCK_REALTIME, &ts);
	struct tm broken_down;
	if (tsType == TS_LOCAL) {
#ifdef WIN32
		localtime_s(&broken_down, &ts.tv_sec);
#else
		localtime_r(&ts.tv_sec, &broken_down);
#endif
	} else if (tsType == TS_UTC) {
#ifdef WIN32
		gmtime_s(&broken_down, &ts.tv_sec);
#else
		gmtime_r(&ts.tv_sec, &broken_down);
#endif
	}
	strftime(buf, bufSz, "%Y/%m/%d %H:%M:%S",
		&broken_down);
	const size_t l = strlen(buf);
	if (l < bufSz) {
		snprintf(buf + l, bufSz - l,
			".%06ld",
			ts.tv_nsec/1000);
	}
	return buf;
}

static const char* put_timestampnum(char* buf, size_t bufSz, TsType tsType)
{
	struct timespec ts;
	csnip_x_clock_gettime(tsType == TS_MONO ? CLOCK_MONOTONIC : CLOCK_REALTIME,
		&ts);
	double ts_sec;
	time_Convert(ts, ts_sec);
	snprintf(buf, bufSz, "%.17g", ts_sec);
	return buf;
}

static const char* value_for_key(const char* keyStart,
		   const char* keyEnd,
		   int prio,
		   const char* comp,
		   const char* src_filepath,
		   const char* src_file,
		   const char* src_func,
		   int src_line,
		   const char* msgformat,
		   va_list ap,
		   char* buf,
		   size_t bufSz)
{
	const size_t keyLen = keyEnd - keyStart;

	switch (keyLen) {
	case 3:
		if (strncmp(keyStart, "msg", 3) == 0) {
			vsnprintf(buf, bufSz, msgformat, ap);
			return buf;
		}
		break;
	case 4:
		if (strncmp(keyStart, "comp", 4) == 0) {
			return comp;
		} if (strncmp(keyStart, "file", 4) == 0) {
			return src_file;
		} else if (strncmp(keyStart, "line", 4) == 0) {
			snprintf(buf, bufSz, "%d", src_line);
			return buf;
		} else if (strncmp(keyStart, "func", 4) == 0) {
			return src_func;
		} else if (strncmp(keyStart, "prio", 4) == 0) {
			snprintf(buf, bufSz, "%d", prio);
			return buf;
		}
		break;
	case 7:
		if (strncmp(keyStart, "timesec", 7) == 0) {
			struct timespec ts;
			csnip_x_clock_gettime(CLOCK_MONOTONIC, &ts);
			snprintf(buf, bufSz, "%.16g",
			  ts.tv_sec + ts.tv_nsec/1e9);
			return buf;
		} else if (strncmp(keyStart, "utctime", 7) == 0) {
			return put_timestamp(buf, bufSz, 0);
		}
		break;
	case 8:
		if (strncmp(keyStart, "filepath", 8) == 0) {
			return src_filepath;
		} else if (strncmp(keyStart, "prioname", 8) == 0) {
			switch(prio / 10) {
#define c(v) case LOG_PRIO_ ## v / 10: return #v;
			c(DEBUGV)
			c(DEBUG)
			c(INFO)
			c(NOTICE)
			c(WARN)
			c(ERR)
#undef c
			default:  return "invalid priority";
			};
		} else if (strncmp(keyStart, "strerror", 8) == 0) {
			x_strerror_r(errno, buf, bufSz);
			return buf;
		}

		break;
	case 9:
		if (strncmp(keyStart, "localtime", 7) == 0)
			return put_timestamp(buf, bufSz, 1);

		break;
	case 10:
		if (strncmp(keyStart, "utctimenum", 10) == 0)
			return put_timestampnum(buf, bufSz, 0);
		break;
	case 11:
		if (strncmp(keyStart, "monotimenum", 11) == 0) {
			return put_timestampnum(buf, bufSz, 2);
		}
		break;
	};

	/* No matching key found */
	snprintf(buf, bufSz, "[INVALID KEY]");
	return buf;
}

void csnip_log__print(
		int style,
		int prio,
		const char* component,
		const char* src_filepath,
		const char* src_file,
		const char* src_func,
		int src_line,
		const char* msgformat,
		...)
{
	const int errno_save = errno;
	if (proc == NULL)
		csnip_log_config0(NULL, NULL);
	csnip_log_processor* P = proc;

	/* Check whether to display */
#ifdef CSNIP_CONF__SUPPORT_THREADING
	pthread_rwlock_rdlock(&P->lock);
#endif
	comp_prio* cp = ptbl_find(proc->ptbl, component);
#ifdef CSNIP_CONF__SUPPORT_THREADING
	pthread_rwlock_unlock(&P->lock);
#endif
	if (cp != NULL) {
		if (prio < cp->min_prio)
			return;
	} else {
		/* Compute the Component minimum priority */
		int comp_min_prio = P->min_prio;
		for (struct filter_rule_S* rule = P->rules_head;
			rule != NULL;
			rule = rule->next)
		{
#ifdef CSNIP_CONF__HAVE_REGCOMP
			const bool match \
			  = (regexec(&rule->re, component, 0, NULL, 0) == 0);
#else
			const bool match \
			  = (strstr(component, rule->substr) != NULL);
#endif
			if (match) {
				if (rule->prio < comp_min_prio) {
					comp_min_prio = rule->prio;
				}
			}
		}

		/* Insert into hash for future lookup */
		comp_prio Pent = {
			.component = component,
			.min_prio = comp_min_prio
		};
#ifdef CSNIP_CONF__SUPPORT_THREADING
		pthread_rwlock_wrlock(&P->lock);
#endif
		ptbl_insert(P->ptbl, NULL, Pent);
#ifdef CSNIP_CONF__SUPPORT_THREADING
		pthread_rwlock_unlock(&P->lock);
#endif

		/* Check if we should display */
		if (prio < Pent.min_prio)
			return;
	}

	/* Format the log message */
	char outBuf[512];
	char* outp = outBuf;
	if (P->logfmt[style] == NULL) {
		/* If no log format string has been specified,
		 * we use the classical, simple, formatting method.
		 */
		do {
			/* Msg prefix */
			int rem = (int)Static_len(outBuf);
			int s = snprintf(outp, rem, "[%s] ", component);
			if (s >= rem)
				break;
			rem -= s;
			outp += s;

			/* Message itself */
			va_list ap;
			va_start(ap, msgformat);
			s = vsnprintf(outp, rem, msgformat, ap);
			va_end(ap);
			if (s >= rem)
				break;
			rem -= s;
			outp += s;

			/* Msg trailer */
			if (style == 1) {
				char buf[512];
				x_strerror_r(errno_save, buf, sizeof buf);
				s = snprintf(outp, rem, ": %s", buf);
				if (s >= rem)
					break;
				rem -= s;
				outp += s;
			}
		} while(0);
	} else {
		/* Method based on format string */

		char valBuf[512];

		#define push(c)	do { \
				if (outp < outBuf + sizeof(outBuf) - 1) \
					*outp++ = (c); \
			} while(0)
		#define scan_key(start, end, p) do { \
				end = strchr(start, '}'); \
				p = end + 1; \
			} while (0)
		#define get_val(start, end, val) do { \
				va_list ap; \
				va_start(ap, msgformat); \
				val = value_for_key(start, \
						end, \
						prio, \
						component, \
						src_filepath, \
						src_file, \
						src_func, \
						src_line, \
						msgformat, \
						ap, \
						valBuf, \
						sizeof valBuf); \
				va_end(ap); \
			} while (0)

		fmt_Gen(char,			// ctype
			P->logfmt[style],	// format string
			'{',			// key char
			start, end, val, p,	// dummy variables
			push(*p),		// push char
			scan_key(start, end, p), // scan key
			get_val(start, end, val), // get value
			(void)val,		// free the value
			_);			// error return

		#undef get_val
		#undef lookup
		#undef scan_key
		#undef push

		/* Need to append '\0' in case we exceeded the buffer
		 * capacity.
		 */
		*outp = '\0';
	}

	/* Display the log message */
	if (P->fp) {
#ifdef CSNIP_CONF__HAVE_FLOCKFILE
		flockfile(P->fp);
#endif
		fputs(outBuf, P->fp);
		fputc('\n', P->fp);
#ifdef CSNIP_CONF__HAVE_FLOCKFILE
		funlockfile(P->fp);
#endif
	}
}
