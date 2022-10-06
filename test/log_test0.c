#include <stdbool.h>
#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/clopts.h>
#include <csnip/log.h>

#define m(prio) \
	do { \
		csnip_log_Mesg(prio, "priority %d", prio); \
		csnip_log_MesgForComp(CSNIP_LOG_COMPONENT \
		  "/wComp", prio, "priority " #prio); \
	} while (0)

#define def_func(fname) \
	static void fname(void) \
	{ \
		m(0); \
		m(10); \
		m(20); \
		m(30); \
		m(40); \
		m(50); \
	}

#define CSNIP_LOG_COMPONENT	"log/funcs/f"
def_func(f)
#undef CSNIP_LOG_COMPONENT

#define CSNIP_LOG_COMPONENT	"log/funcs/g"
def_func(g)
#undef CSNIP_LOG_COMPONENT

#define CSNIP_LOG_COMPONENT	"log/funcs/h"
def_func(h)
#undef CSNIP_LOG_COMPONENT

int main(int argc, char** argv)
{
	bool uninitialized = false;
	bool config0 = false;

	csnip_log_configuration C = { NULL };

	clopts opts = {
	  .description = "Log component test."
	};
	clopts_add_defaults(&opts);
	clopts_Addflag(&opts, 'u', "uninitialized",
	  "skip csnip_log_config()", &uninitialized, _);
	clopts_Addflag(&opts, '0', "config0",
	  "use csnip_log_config0 instead of csnip_log_config",
	  &config0, _);

	clopts_Addvar(&opts, 'e', "filter-expr", "filter expression",
		&C.filter_expr, _);
	clopts_Addvar(&opts, 'f', "format", "log format",
		&C.logfmt[0], _);

	if (csnip_clopts_process(&opts, argc - 1, argv + 1, NULL, true) != 0) {
		return 1;
	}

	/* Initialize logger */
	if (!uninitialized) {
		if (config0) {
			csnip_log_config0(C.filter_expr, stdout);
		} else {
			csnip_log_config(&C);
		}
	}

	/* Call the funcs */
	f();
	g();
	h();

	return 0;
}
