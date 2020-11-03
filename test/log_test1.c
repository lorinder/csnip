#define CSNIP_SHORT_NAMES
#include <csnip/log.h>

#define CSNIP_LOG_COMPONENT	"TestComponent"

int main(int argc, char** argv)
{
	log_Perror(LOG_PRIO_NOTICE, "perror prefix");

	return 0;
}
