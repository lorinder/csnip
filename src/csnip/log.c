#include <csnip/csnip_conf.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#ifdef CSNIP_CONF__HAVE_REGCOMP
#include <regex.h>
#endif
#include <pthread.h>

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/list.h>
#include <csnip/log.h>
#include <csnip/lphash_table.h>
#include <csnip/mem.h>
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
	pthread_rwlock_t lock;

	/** Priorities cache */
	struct priotbl* ptbl;

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
	pthread_rwlock_init(&P->lock, NULL);
	P->ptbl = ptbl_make(&err); // error handling
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
	pthread_rwlock_destroy(&P->lock);

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
			ent->substr = strdup(expr);
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

void csnip_log_config0(const char* filter_expr,
		FILE* log_out_)
{
	/* Clear out old, if any */
	if (proc) {
		proc_free(proc);
		proc = NULL;
	}

	mem_Alloc(1, proc, _);
	proc_init(proc);

	/* Create the `re` array */
	if (filter_expr != NULL) {
		proc->min_prio = 100;

		char* fe2 = strdup(filter_expr);
		proc_add_filters(proc, fe2);
		free(fe2);
	} else {
		proc->min_prio = PRIO_DEFAULT;
	}

	/* Set the log file target */
	proc->fp = (log_out_ ? log_out_ : stderr);
}

void csnip_log_free(void)
{
	if (proc != 0) {
		proc_free(proc);
		proc = NULL;
	}
}

void csnip_log__mesg_trailer(FILE* fp)
{
	fputc('\n', fp);
}

void csnip_log__perror_trailer(FILE* fp)
{
	char buf[1024];
	x_strerror_r(errno, buf, sizeof(buf));
	fprintf(fp, ": %s\n", buf);
}

void csnip_log__print(
		void (*print_trailer)(FILE*),
		int prio,
		const char* src_file,
		int src_line,
		const char* src_func,
		const char* component,
		const char* format,
		...)
{
	const int errno_save = errno;
	if (proc == NULL)
		csnip_log_config0(NULL, NULL);
	csnip_log_processor* P = proc;

	/* Check whether to display */
	pthread_rwlock_rdlock(&P->lock);
	comp_prio* cp = ptbl_find(proc->ptbl, component);
	pthread_rwlock_unlock(&P->lock);
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
		pthread_rwlock_wrlock(&P->lock);
		ptbl_insert(P->ptbl, NULL, Pent);
		pthread_rwlock_unlock(&P->lock);

		/* Check if we should display */
		if (prio < Pent.min_prio)
			return;
	}

	/* Display log entry */
#ifdef CSNIP_CONF__HAVE_FLOCKFILE
	flockfile(P->fp);
#endif
	if (P->fp) {
		/* Msg prefix */
		fprintf(P->fp, "%s:%d: [%s] ",
		  src_file,
		  src_line,
		  component);

		/* Message itself */
		va_list ap;
		va_start(ap, format);
		vfprintf(P->fp, format, ap);
		va_end(ap);

		/* Msg trailer */
		errno = errno_save;
		(*print_trailer)(P->fp);
	}
#ifdef CSNIP_CONF__HAVE_FLOCKFILE
	funlockfile(P->fp);
#endif
}
