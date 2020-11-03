#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>
#include <csnip/list.h>
#include <csnip/util.h>

struct Entry {
	const char* name;
	struct Entry *prev, *next;
};

int main(void)
{
	// Create an empty list:
	struct Entry *head, *tail;
	dlist_Init(head, tail, prev, next);

	// Add some stuff to it
	const char* names_to_add[] = {
	  "John", "David", "Michael", "Scott"
	};
	for (int i = 0; i < Static_len(names_to_add); ++i) {
		struct Entry* enew;
		mem_Alloc(1, enew, _);
		enew->name = names_to_add[i];

		// Add the entry
		dlist_PushTail(head, tail, prev, next, enew);
	}

	// Now read out backwards
	struct Entry* p = tail;
	while (p) {
		printf("%s\n", p->name);
		p = p->prev;
	}

	return 0;
}
