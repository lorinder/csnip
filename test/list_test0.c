#include <stdio.h>
#include <stdlib.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>
#include <csnip/list.h>
#include <csnip/cext.h>

typedef struct Ent_s {
	int val;
	struct Ent_s *ptr_prev, *ptr_next;
} Ent;

typedef struct {
	Ent *head, *tail;
} EntList;

CSNIP_DLIST_DECL_FUNCS(static cext_unused, entlist_, Ent*, args(EntList*))
CSNIP_DLIST_DEF_FUNCS(static, entlist_, Ent*, args(EntList* list),
	list->head, list->tail, ptr_prev, ptr_next);

void fill(EntList* L, int n)
{
	for (int i = 0; i < n; ++i) {
		Ent* e;
		mem_Alloc(1, e, _);
		*e = (Ent){ .val = i };
		entlist_push_tail(L, e);
	}
}

int main()
{
	EntList L;
	entlist_init(&L);
	fill(&L, 10);

	printf("List: ");
	for(Ent* p = entlist_head(&L); p; p = entlist_next(p)) {
		printf("%d -> ", p->val);
	}
	puts("NULL");

	return 0;
}
