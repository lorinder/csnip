#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <csnip/cext.h>
#include <csnip/hash.h>
#include <csnip/lphash_table.h>
#include <csnip/search.h>
#include <csnip/sort.h>
#include <csnip/util.h>

#define always_assert(expr)  do { \
	if (!(expr)) { \
		fprintf(stderr, "Check failed: \"" #expr "\". Abort.\n"); \
		exit(1); \
	} \
	} while (0)


/* Integer hashing table */

static void u32arr_sort(uint32_t* arr, size_t N)
{
	csnip_Qsort(a, b,
		arr[a] < arr[b],
		csnip_Tswap(uint32_t, arr[a], arr[b]),
		N);
}

static _Bool u32_sorted_arr_contains(uint32_t* arr, size_t N, uint32_t val)
{
	size_t r;
	csnip_Bsearch(size_t, u, arr[u] < val, N, r);
	assert(r <= N);
	if (r == N)
		return 0;
	if (arr[r] != val)
		return 0;
	return 1;
}

/* hash function source: http://burtleburtle.net/bob/hash/integer.html */
static uint32_t u32hash( uint32_t a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

CSNIP_LPHASH_TABLE_DEF_TYPE(u32set,	// struct table type
			uint32_t)	// entry type
CSNIP_LPHASH_TABLE_DECL_FUNCS(static,	// scope,
			u32set_,	// prefix
			uint32_t,	// key type
			uint32_t,	// entry type
			struct u32set)	// table type
CSNIP_LPHASH_TABLE_DEF_FUNCS(static csnip_cext_unused, // scope
			u32set_,	// prefix
			uint32_t,	// key type
			uint32_t,	// entry type
			struct u32set,	// table type
			k1, k2, e,	// dummy vars (keys + entry)
			u32hash(k1),	// hashing of k1
			k1 == k2,	// is_match
			e)		// get_key

_Bool intset_test0()
{
	static const uint32_t v[] = {
	    747, 34,872,913,857,754,878, 28, 94,546,919,549,664,118,149,
	    881,138,888,494,508,535,764,887, 27, 25, 42,438,832,846,755,
	    680,763, 85,917,990,171,411,503,653,982,651,130,441,472,102,
	    244,565, 79,221,710,601,563,595,301,713,150,621,792,989,675,
	    193,920,245,457,670,134,360,163,258, 32,307,148,665,607,276,
	    336,157,455,484,635,272,328,393,530,568,528,380,574,211,696,
	    442,782,668,757,952,169,625,243,801,912,385,906,800,728, 53,
	    488, 68,842,176,905,483,667,440,581,583,828,707,604,133,767,
	    332,529,224,947,610,106,843,975,299,518,889,519,615,391,626,
	    376,752,491,984,449,624,885,320,773,648,831,226,899,433, 18,
	    942,349,481,120, 72,100,871,284,566,893,907,633,795,  7,727,
	    329,397,458, 74,182,228,848,704,409,944,690,884,536,669,596,
	    577,288,426,356,770,561,394,119,140,165,698,721,806,423,377,
	    765,429,192,783,591,117,295,456, 90,693,762, 14,362,949,758,
	    238,638,943,451,520,400,709, 43,340,414,998,930,643,168,585,
	    127,904,179,431,225,222,359,342,641,979,688,729,799,932,427,
	     59,983,159,677,395,420,374,334,973,895,319,981,903,790,742,
	    852,352, 16,658,507,582,866,390,600,375,128,802,399,836,636,
	    750,185,477,223,255,617,955,575,239,813,746,379,298,545,874,
	    662,963,951,538,547,628,570,901,941,593,161,833,964,618,918,
	    246,129,965, 67,956, 81,855,586,351,810, 73,732,419, 77, 57,
	    260,469,731,748,268,144,280,647,203,407, 66,164,432,447,776,
	     49,152,396, 24,553,384,523,514,317,978,760,873,347,154, 84,
	    991,147,210,556,809,835,476,815,865,338,105,862,692,603,787,
	    953,847,459,860,270,616,461,850,559,197,910,737,686, 83,425,
	    808,200,166,909, 62, 19,274,242,446,640,170,969,462, 52,542,
	    205,576,826, 95,834, 98,504,386,125,807,614,572,401, 45,418,
	    948,263,343,730,980,413,829,408,314,622,398,146,937,326, 17,
	    373,780,551, 20, 37,612,  3,946,107,450,608,  1,839,173,598,
	    435,701,870,293,723,370,864,988,495,473,819,814,303,714,637,
	    939,772,123,816,926,940,875,277, 78,404,502,521,321,958,124,
	    718,970,464,  8, 65,155,522,448,791, 47,410,143,890,348,465,
	    891,650,892,992,685,623,252,540,344,172,703,524,  4,217,114,
	};
	const int N = csnip_Static_len(v);
	uint32_t* sorted_v = NULL;

	struct u32set* set = u32set_make(NULL);
	_Bool success = 1;

	/* Populate with a complete set of values */
	for (int i = 0; i < N; ++i) {
		_Bool s = u32set_insert(set, NULL, v[i]);
		if (!s) {
			fprintf(stderr, "Error:  Insertion of %" PRIu32 " failed (i = %d)\n",
			  v[i], i);
			success = 0;
			goto done;
		}
	}

	/* Print some intermediate results */
	printf(" After insertion of %d integers: size=%zu, capacity=%zu, load factor = %g\n",
		N, u32set_size(set), u32set_capacity(set),
		u32set_size(set) / (double)u32set_capacity(set));
	if (u32set_size(set) != N) {
		fprintf(stderr, "Error:  Wrong set size.\n");
		success = 0;
		goto done;
	}
	if (u32set_capacity(set) < u32set_size(set)) {
		fprintf(stderr, "Error:  Too small capacity.\n");
	}

	/* Reinserting members of v should not work */
	for (int i = 0; i < N; ++i) {
		if (u32set_insert(set, NULL, v[i])) {
			fprintf(stderr, "Error:  Insertion of %" PRIu32 " worked, when it was "
			   "supposedly already present (i = %d)\n", v[i], i);
			success = 0;
			goto done;
		}
	}
	puts(" Reinserting members of v did not work, as expected");

	/* Creating a sorted version of v */
	csnip_mem_Alloc(N, sorted_v, _);
	csnip_Copy_n(v, N, sorted_v);
	u32arr_sort(sorted_v, N);

	/* We should be able to find all members of v */
	for (int i = 0; i < N; ++i) {
		uint32_t* p = u32set_find(set, sorted_v[i]);
		if (p == 0) {
			fprintf(stderr, "Error:  Value %" PRIu32" not found in table "
					"(i = %d)\n", sorted_v[i], i);
			success = 0;
			goto done;
		}
		if (*p != sorted_v[i]) {
			fprintf(stderr, "Error:  Find returns a mistaken entry.\n");
			fprintf(stderr, "        Looking for %" PRIu32 ", found %"PRIu32"\n",
				sorted_v[i], *p);
			success = 0;
			goto done;
		}
	}
	puts(" All members of v could be found in the table");

	/* We should not find any non-member of v */
	{
		int j = 0, ctr = 0;
		for (int i = 0; i < 1000; ++i) {
			if (sorted_v[j] == i) {
				++j;
				if (j == N)
					break;
				continue;
			}

			/* j is a non-member */
			++ctr;
			uint32_t *p = u32set_find(set, i);
			if (p) {
				fprintf(stderr, "Error:  Unexpectedly found non-member %d (j = %d)\n",
				  i, j);
				success = 0;
				goto done;
			}
		}

		printf(" No non-members were found, %d were inspected.\n", ctr);
	}

	/* Count the number of non-occupied slots */
	{
		size_t ctr = 0;
		for (int i = 0; i < u32set_capacity(set); ++i) {
			if (!u32set_isslotoccupied(set, i)) {
				++ctr;
			}
		}
		printf(" Found %zu unoccupied slots in %zu slots total.\n",
			ctr, u32set_capacity(set));
		if (ctr + u32set_size(set) != u32set_capacity(set)) {
			fprintf(stderr, "Error: Size and unoccupied slots "
			  "don't add up to capacity\n");
			success = 0;
			goto done;
		}
	}

	/* Iterate through the table */
	{
		int ctr = 0;
		for (size_t e = u32set_firstoccupiedslot(set);
			e < u32set_capacity(set);
			e = u32set_nextoccupiedslot(set, e))
		{
			++ctr;
			always_assert(u32set_isslotoccupied(set, e));
			uint32_t val = *u32set_getslotentryaddress(set, e);
			if (!u32_sorted_arr_contains(sorted_v, N, val)) {
				fprintf(stderr, "Error: Iteration yields unknown value "
				  "%"PRIu32" (e = %zu)\n", val, e);
				success = 0;
				goto done;
			}
		}

		printf(" Iteration over all members -> %d values inspected\n", ctr);
		if (ctr != N) {
			fprintf(stderr, "Error: Iteration over wrong number of members\n");
			success = 0;
			goto done;
		}

	}

	/* Remove half the members with remove() */
	for (int i = 0; i < N; i += 2) {
		_Bool result = u32set_remove(set, NULL, v[i]);
		if (!result) {
			fprintf(stderr, "Error: Could not remove %"PRIu32
				" (i == %d)\n", v[i], i);
			success = 0;
			goto done;
		}
	}
	printf(" Members remaining after removing %d: %zu\n",
		(N + 1)/2, u32set_size(set));
	if (u32set_size(set) + (N + 1)/2 != N) {
		fprintf(stderr, "Error: Unexpected size.\n");
		success = 0;
		goto done;
	}

	/* Add all the members with find_or_insert() */
	for (int i = 0; i < N; ++i) {
		uint32_t* e = u32set_find_or_insert(set, NULL, sorted_v[i]);
		always_assert(*e == sorted_v[i]);
	}
	printf(" Readded members; now there are %zu\n", u32set_size(set));
	if (u32set_size(set) != N) {
		fprintf(stderr, "Error: Unexpected size.\n");
		success = 0;
		goto done;
	}

	/* Removal of all the members via removeatslot() */
	{
		int ctr = 0;
		for (size_t e = u32set_firstoccupiedslot(set);
			e < u32set_capacity(set);
			e = u32set_removeatslot(set, NULL, e))
		{
			always_assert(u32set_isslotoccupied(set, e));
			++ctr;
		}
		printf(" After removal of each member (%d iterations), size=%zu\n",
			ctr, u32set_size(set));
		if (ctr != N) {
			fprintf(stderr, "Error: Number of iterations does not match N\n");
			success = 0;
			goto done;
		}
		if (u32set_size(set) != 0) {
			fprintf(stderr, "Error: Remaining size != 0\n");
			success = 0;
			goto done;
		}
	}

	/* Finish */
done:
	if (sorted_v)
		csnip_mem_Free(sorted_v);
	u32set_free(set);
	return success;
}

/* Set of C strings */

CSNIP_LPHASH_TABLE_DEF_TYPE(cstrset, const char*)
CSNIP_LPHASH_TABLE_DECL_FUNCS(static, cstrset_,
				const char*, const char*, struct cstrset)
CSNIP_LPHASH_TABLE_DEF_FUNCS(static csnip_cext_unused, cstrset_,
				const char*, const char*, struct cstrset,
				k1, k2, e,
				csnip_hash_fnv32_s(k1, CSNIP_FNV32_INIT),
				strcmp(k1, k2) == 0,
				e)

static void cstrarr_sort(const char** arr, size_t N)
{
	csnip_Qsort(a, b,
		strcmp(arr[a], arr[b]) < 0,
		csnip_Tswap(const char*, arr[a], arr[b]),
		N);
}

static _Bool cstr_sorted_arr_contains(const char** arr, size_t N, const char* val)
{
	size_t r;
	csnip_Bsearch(size_t, u, strcmp(arr[u], val) < 0, N, r);
	always_assert(r <= N);
	if (r == N)
		return 0;
	if (strcmp(arr[r], val) != 0)
		return 0;
	return 1;
}

_Bool cstrset_test1()
{
	static const char* v[] = {
	"Eva", "Americanized", "rotational", "germicidal", "fitting",
	"Aurelius", "reoccur", "pottier", "proliferating", "ardor",
	"planning", "Nimitz", "Manuela", "comatose", "pinwheel", "parch",
	"retreading", "rightist", "Lepidus", "glorying", "prawn", "girding",
	"prow", "geologic", "ivories", "Okhotsk", "wakefulness",
	"immigrant", "trucked", "psychiatric", "equivocally", "philander",
	"Democrat", "goldenest", "cavern", "resister", "ganging",
	"Visigoth", "centrist", "untidy", "moratorium", "Tracey", "Gina",
	"everyone", "damasked", "overmuches", "drunk", "unlatches",
	"consign", "pinwheeling", "Sakharov", "psychotherapist",
	"syncopated", "Tesla", "plaguing", "dubiousness", "nostril",
	"Shane", "review", "damnedest", "trickiness", "ranted", "sludge",
	"junkier", "Tigris", "warren", "diplomat", "motionless", "Tunney",
	"innumerable", "Mendelian", "cathode", "toadied", "increased",
	"institutionalized", "deescalate", "wobbly", "persona", "toil",
	"flies", "hided", "virago", "Gruyeres", "uncharted", "filmiest",
	"consummated", "battery", "Telemann", "propagandized", "deflating",
	"twiggiest", "zanier", "upturn", "dominant", "vow", "softener",
	"Jekyll", "regular", "q", "deported", "defused", "hurricane",
	"prediction", "Hutu", "strewed", "voicemail", "filed", "Marple",
	"glazing", "Tutsi", "rather", "reversible", "overanxious",
	"Minerva", "gusher", "sager", "revving", "tinkling", "bridling",
	"scrawniest", "yam", "candied", "stopper", "chipping", "decorate",
	"sluicing", "primped", "taxi", "tankard", "splashed", "scapulae",
	"gallium", "aquiline", "telecommuting", "delinquently", "coward",
	"extricate", "vising", "workaday", "Texaco", "permissibly",
	"Hemingway", "unwary", "shameless", "gynecological", "quell",
	"smidgen", "Ac", "Cameroon", "starting", "Rheingau",
	"nonprescription", "loopiest", "trifocals", "Galveston", "coat",
	"Knight", "Angora", "Volkswagen", "Richthofen", "broil", "choppier",
	"biting", "psychic", "lightninged", "supplementing", "Lodz", "peel",
	"aircraft", "liefest", "indexing", "speciously", "exaggeration",
	"Sarajevo", "Hector", "neat", "inscrutable", "akimbo",
	"typographer", "quorum", "Bilbo", "cushion", "Modigliani",
	"dapperest", "chicest", "elision", "prided", "antihero", "signpost",
	"salary", "jeremiad", "perfidy", "prettily", "annoyed", "slot",
	"urbanization", "Bloomsbury", "culotte", "spidery", "lea", "hanged",
	"neighborhood", "marginally", "corralled", "normalize",
	"instantaneous", "lexical", "torrid", "kneeing", "geekier", "Cisco",
	"Methuselah", "infringing", "gosh", "Ezra", "glaringly", "roamer",
	"haberdasheries", "clinical", "bloodying", "quizzing", "voiceless",
	"petrel", "dereliction", "sponging", "Jude", "Iapetus", "Motown",
	"approximation", "Celeste", "Alyson", "marker", "manumitting",
	"standoff", "loco", "laterally", "Connecticut", "adaptation",
	"tabooed", "thief", "Sand", "vituperative", "Alnitak", "indent",
	"angler", "bide", "minuend", "quisling", "physiognomies", "farm",
	"Zane", "scummy", "paleontologist", "pended", "polysyllabic",
	"Kinshasa", "decolonized", "clerking", "whammies", "unmistakably",
	"calypso", "postured", "tattle", "operating", "Edmond", "phonology",
	"Marquita", "Glasgow", "articulated", "Gray", "quadrangular",
	"Saki", "manicure", "diddled", "expansion", "uninformed", "grouch",
	"gauge", "rescuing", "prouder", "shopping", "waywardness",
	"scallop", "cosign", "vasectomy", "bamboozle", "swearer", "gunshot",
	"ionizing", "foxed", "outrank", "venerable", "unnerve",
	"discharging", "alliance", "answered", "wheelbarrow", "condenser",
	"untenable", "spurned", "gritted", "demoralizing", "kind",
	"quadruplicated", "bile", "interact", "buckshot", "gassed",
	"repackaged", "unceasing", "graven", "nappiest", "gross", "compose",
	"upfront", "breadth", "bishopric", "mathematics", "porn",
	"accordion", "Viking", "burr", "eucalyptuses", "dissidence",
	"linchpin", "handcrafted", "Alissa", "consequence", "breadbasket",
	"peerless", "unsold", "phisher", "maturing", "hatefulness",
	"hoofed", "plighting", "girdling", "Jill", "rattler", "client",
	"Hiss", "excelling", "brainwashes", "humongous", "abstained",
	"Wright", "prickle", "Quirinal", "cola", "contrite", "torte",
	"complaining", "parked", "snakiest", "Quaoar", "eked", "hooped",
	"tureen", "yoked", "sow", "thin", "indulging", "horsehide", "crib",
	"Independence", "Todd", "theologies", "charred", "deplane",
	"mosaic", "weighty", "vignetted", "entanglement", "unfrequented",
	"shocked", "Tulsa", "distribute", "popularizing", "Taoism",
	"sentimentalist", "indecorous", "whiplashes", "adverbial", "Schick",
	"sodium", "militiamen", "Corning", "passageway", "rolling",
	"keeping", "competitively", "helicopter", "spring", "posit",
	"Bangor", "asked", "satirist", "misinterpret", "incorporeal",
	"enriching", "petering", "relapse", "loomed", "autocrat", "Sue",
	"masterfully", "fructose", "elongation", "webcast", "hardtack",
	"Chartres", "Orin", "inhumanely", "Gaea", "jostled", "excitement",
	"straining", "fuzziness", "apter", "Golan", "harridan",
	"striptease", "crowning", "revue", "checkering", "Pablum",
	"tightest", "Oberlin", "repudiate", "Carboloy", "sublease",
	"guaranteeing", "pecked", "feller", "Bambi", "Elnath", "id",
	"Crest", "Stockholm", "slather", "bathos", "Sunbeam", "dispirited",
	"Poland", "Burr", "cabinetmaker", "Kagoshima", "East", "Meyerbeer",
	"Michigan", "brawniest", "reassurance", "forge", "pigged", "trench",
	"Pantaloon", "punishing", "Gallic", "heavier", "Izvestia", "cold",
	"serener", "heritage", "hotbed", "Southey", "chimaera", "cranny",
	"ogre", "delicately", "reformation", "wiretap", "preppy",
	"apostasy", "Indira", "fuzzily", "swamping", "loaning", "Cunard",
	"amoebae", "skyjack", "chronicler", "blazing", "deliciously",
	"foursquare", "mending", "highest", "assembly", "nationality",
	"comparatively", "scruff", "irreconcilable", "cauliflower",
	"insurance", "proportionality", "sorter", "meadow", "capsuling",
	"ratifying", "proffered", "dangerously"
	};
	const int N = csnip_Static_len(v);
	const char** sorted_v = NULL;

	struct cstrset* set = cstrset_make(NULL);
	_Bool success = 1;

	/* Populate with a complete set of values */
	for (int i = 0; i < N; ++i) {
		_Bool s = cstrset_insert(set, NULL, strdup(v[i]));
		if (!s) {
			fprintf(stderr, "Error:  Insertion of \"%s\" failed (i = %d)\n",
			  v[i], i);
			success = 0;
			goto done;
		}
	}

	/* Print some intermediate results */
	printf(" After insertion of %zu strings: size=%zu, capacity=%zu, load factor = %g\n",
		csnip_Static_len(v), cstrset_size(set), cstrset_capacity(set),
		cstrset_size(set) / (double)cstrset_capacity(set));
	if (cstrset_size(set) != csnip_Static_len(v)) {
		fprintf(stderr, "Error:  Wrong set size.\n");
		success = 0;
		goto done;
	}
	if (cstrset_capacity(set) < cstrset_size(set)) {
		fprintf(stderr, "Error:  Too small capacity.\n");
	}

	/* Reinserting members of v should not work */
	for (int i = 0; i < N; ++i) {
		char* d = strdup(v[i]);
		if (cstrset_insert(set, NULL, d)) {
			fprintf(stderr, "Error:  Insertion of \"%s\" worked, when it was "
			   "supposedly already present (i = %d)\n", v[i], i);
			success = 0;
			goto done;
		} else {
			free(d);
		}
	}
	puts(" Reinserting members of v did not work, as expected");

	/* Creating a sorted version of v */
	csnip_mem_Alloc(N, sorted_v, _);
	csnip_Copy_n(v, N, sorted_v);
	cstrarr_sort(sorted_v, N);

	/* We should be able to find all members of v */
	for (int i = 0; i < N; ++i) {
		const char** p = cstrset_find(set, sorted_v[i]);
		if (p == NULL) {
			fprintf(stderr, "Error:  String \"%s\" not found in table "
					"(i = %d)\n", sorted_v[i], i);
			success = 0;
			goto done;
		}
		if (strcmp(*p, sorted_v[i]) != 0) {
			fprintf(stderr, "Error:  Find returns a mistaken entry.\n");
			fprintf(stderr, "        Looking for \"%s\", found \"%s\"\n",
				sorted_v[i], *p);
			success = 0;
			goto done;
		}
	}
	puts(" All members of v could be found in the table");

#if 0
	/* We should not find any non-member of v */
	{
		int j = 0, ctr = 0;
		for (int i = 0; i < 1000; ++i) {
			if (sorted_v[j] == i) {
				++j;
				continue;
			}

			/* j is a non-member */
			++ctr;
			char** p = cstr_find(set, i);
			if (p) {
				fprintf(stderr, "Error:  Unexpectedly found non-member %d (j = %d)\n",
				  i, j);
				success = 0;
				goto done;
			}
		}

		printf(" No non-members were found, %d were inspected.\n", ctr);
	}
#endif

	/* Count the number of non-occupied slots */
	{
		size_t ctr = 0;
		for (int i = 0; i < cstrset_capacity(set); ++i) {
			if (!cstrset_isslotoccupied(set, i)) {
				++ctr;
			}
		}
		printf(" Found %zu unoccupied slots in %zu slots total.\n",
			ctr, cstrset_capacity(set));
		if (ctr + cstrset_size(set) != cstrset_capacity(set)) {
			fprintf(stderr, "Error: Size and unoccupied slots "
			  "don't add up to capacity\n");
			success = 0;
			goto done;
		}
	}

	/* Iterate through the table */
	{
		int ctr = 0;
		for (size_t e = cstrset_firstoccupiedslot(set);
			e < cstrset_capacity(set);
			e = cstrset_nextoccupiedslot(set, e))
		{
			++ctr;
			assert(cstrset_isslotoccupied(set, e));
			const char* val = *cstrset_getslotentryaddress(set, e);
			if (!cstr_sorted_arr_contains(sorted_v, N, val)) {
				fprintf(stderr, "Error: Iteration yields unknown value "
				  "\"%s\" (e = %zu)\n", val, e);
				success = 0;
				goto done;
			}
		}

		printf(" Iteration over all members -> %d values inspected\n", ctr);
		if (ctr != N) {
			fprintf(stderr, "Error: Iteration over wrong number of members\n");
			success = 0;
			goto done;
		}

	}

	/* Remove half the members with remove() */
	for (int i = 0; i < N; i += 2) {
		size_t slot = cstrset_findslot(set, v[i]);
		if (slot == cstrset_capacity(set)) {
			fprintf(stderr, "Error: Could not remove \"%s\""
				" (i == %d)\n", v[i], i);
			success = 0;
			goto done;
		} else {
			cstrset_removeatslot(set, NULL, slot);
		}
	}
	printf(" Members remaining after removing %d: %zu\n",
		(N + 1)/2, cstrset_size(set));
	if (cstrset_size(set) + (N + 1)/2 != N) {
		fprintf(stderr, "Error: Unexpected size.\n");
		success = 0;
		goto done;
	}

	/* Add all the members with find_or_insert() */
	for (int i = 0; i < N; ++i) {
		const char** e = cstrset_find_or_insert(set,
					NULL,
					(char*)sorted_v[i]);
		if(*e == sorted_v[i]) {
			/* Insertion happened, copy. */
			*e = strdup(*e);
		}
	}
	printf(" Readded members; now there are %zu\n", cstrset_size(set));
	if (cstrset_size(set) != N) {
		fprintf(stderr, "Error: Unexpected size.\n");
		success = 0;
		goto done;
	}

	/* Removal of all the members via removeatslot() */
	{
		int ctr = 0;
		for (size_t e = cstrset_firstoccupiedslot(set);
			e < cstrset_capacity(set);
			e = cstrset_removeatslot(set, NULL, e))
		{
			assert(cstrset_isslotoccupied(set, e));
			free((char*)*cstrset_getslotentryaddress(set, e));
			++ctr;
		}
		printf(" After removal of each member (%d iterations), size=%zu\n",
			ctr, cstrset_size(set));
		if (ctr != N) {
			fprintf(stderr, "Error: Number of iterations does not match N\n");
			success = 0;
			goto done;
		}
		if (cstrset_size(set) != 0) {
			fprintf(stderr, "Error: Remaining size != 0\n");
			success = 0;
			goto done;
		}
	}

	/* Finish */
done:
	if (sorted_v)
		csnip_mem_Free(sorted_v);
	cstrset_free(set);
	return success;
}

int main()
{
#define RUN_TEST(name) \
	puts("\nRunning test " #name); \
	if (name()) { \
		printf("-> " #name " passed.\n"); \
	} else { \
		printf("-> " #name " failed.\n"); \
		return 1; \
	}

	RUN_TEST(intset_test0)
	RUN_TEST(cstrset_test1)

	return 0;
}
