#include <stdio.h>
#include <string.h>

/*  Another hash table test.
 *
 *  This one does adds a few aspects compared to hashtable_test0.c
 *
 *  - Creates a hash table instead of a hash set (i.e., the entry set
 *    is different from the key set).
 *
 *  - Uses CSNIP_SHORT_NAMES (not that it matters much for
 *    lphash_table).
 *
 *  - Test operates also on an empty, newly created map.  (A bug was
 *    spotted in this case.)
 */

#define CSNIP_SHORT_NAMES
#include <csnip/cext.h>
#include <csnip/lphash_table.h>

typedef struct {
	int key;
	const char* val;
} int2str_entry;

CSNIP_LPHASH_TABLE_DEF_TYPE(int2str_map_s,	// tbltype
			int2str_entry)		// entry type
typedef struct int2str_map_s int2str_map;

CSNIP_LPHASH_TABLE_DEF_FUNCS(cext_unused static, // scope
			int2str_,		// prefix
			int,			// keytype
			int2str_entry,		// entrytype
			int2str_map,		// tbltype
			k1, k2, e,		// dummy vars
			k1,			// hash
			k1 == k2,		// is_match
			e.key)			// get_key

int main(int argc, char** argv)
{
	int2str_map* M = int2str_make(NULL);

	/* Empty table operations */
	puts("Trying some operations on the empty table.");

	int2str_entry* E = int2str_find(M, 0);
	if (E != NULL) {
		fprintf(stderr, "Error:  Unexpectedly found entries "
		  "in empty map.\n");
		return 1;
	}

	if (int2str_size(M) != 0) {
		fprintf(stderr, "Error:  Unexpected nonzero size.\n");
		return 1;
	}

	if (int2str_findslot(M, 1) != int2str_capacity(M)) {
		fprintf(stderr, "Error:  Unexpectedly found a slot "
		  "for non-existent entries.\n");
		return 1;
	}

	/* Check element replacement */
	puts("Testing element replacement in hash table.");

	{
		int2str_entry E = { 5, "James" };
		int2str_insert_or_assign(M, NULL, E, NULL);
		E = (int2str_entry){ 5, "Donald" };
		int2str_insert_or_assign(M, NULL, E, NULL);
	}
	if (int2str_size(M) != 1) {
		fprintf(stderr, "Error:  Unexpected size after "
		  "inserting the same key twice: %zu\n", int2str_size(M));
		return 1;
	}
	E = int2str_find(M, 5);
	if (E == NULL) {
		fprintf(stderr, "Error:  Did not find previously inserted "
		  "entry.\n");
		return 1;
	}
	if (strcmp(E->val, "Donald") != 0) {
		fprintf(stderr, "Error:  Did not find expected key \"Donald\""
		  " for value 5, but \"%s\" instead.\n", E->val);
		return 1;
	}

	puts("-> tests passed.\n");
	return 0;
}

