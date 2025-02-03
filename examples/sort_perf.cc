#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>

#define CSNIP_SHORT_NAMES
#include <csnip/util.h>
#include <csnip/arr.h>
#include <csnip/sort.h>
#include <csnip/x.h>

typedef enum {
	M_STD_SORT,
	M_STD_QSORT,
	M_CSNIP_QSORT,
	M_CSNIP_HEAPSORT,
	M_CSNIP_SHELLSORT,
} sort_method_t;

typedef enum {
	T_RANDOM,
	T_INCREASING,
	T_DECREASING,
	T_DNF,
	T_ALLEQ,
	T_ORGANPIPE,
} task_t;

typedef enum {
	K_INT,
	K_CSTR,
} sortkey_t;

static double get_delta(struct timespec* b, struct timespec* a)
{
	return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec)/1.e9;
}

/* Integer sorting test */

static int intcmp(const void* A, const void* B)
{
	const int* a = (const int*)A;
	const int* b = (const int*)B;
	if (*a < *b)
		return -1;
	if (*a > *b)
		return 1;
	return 0;
}

void create_int_instance(int* arr, int nItem, task_t task)
{
	switch (task) {
	case T_RANDOM:
		for (int j = 0; j < nItem; ++j) {
			arr[j] = std::rand();
		}
		break;
	case T_INCREASING:
		for (int j = 0; j < nItem; ++j) {
			arr[j] = j;
		}
		break;
	case T_DECREASING:
		for (int j = 0; j < nItem; ++j) {
			arr[j] = nItem - j;
		}
		break;
	case T_DNF:
		for (int j = 0; j < nItem; ++j) {
			const int u = int(4. * std::rand() / (RAND_MAX + 1.0));
			arr[j] = u;
		}
		break;
	case T_ALLEQ:
		for (int j = 0; j < nItem; ++j) {
			arr[j] = 0;
		}
		break;
	case T_ORGANPIPE:
		for (int j = 0; 2*j <= nItem; ++j) {
			arr[j] = j;
			arr[nItem - j - 1] = j;
		}
		break;
	};
}

void sort_int_instance(int* arr,
			int nItem,
			sort_method_t meth)
{
	switch (meth) {
	case M_STD_SORT:
		std::sort(arr, &arr[nItem]);
		break;
	case M_STD_QSORT:
		std::qsort(arr, nItem,
		  sizeof(arr[0]), intcmp);
		break;
	case M_CSNIP_QSORT:
		csnip_Qsort(u, v,
			arr[u] < arr[v],
			csnip_Tswap(int, arr[u], arr[v]),
			nItem);
		break;
	case M_CSNIP_HEAPSORT:
		csnip_Heapsort(u, v,
			arr[u] < arr[v],
			csnip_Tswap(int, arr[u], arr[v]),
			nItem);
		break;
	case M_CSNIP_SHELLSORT:
		csnip_Shellsort(u, v,
			arr[u] < arr[v],
			csnip_Tswap(int, arr[u], arr[v]),
			nItem);
		break;
	};
}

void check_int_instance(const int* arr,
			int nItem)
{
	int result;
	csnip_IsSorted(u, v, arr[u] < arr[v],
	  nItem, result);
	if (!result) {
		fprintf(stderr, "%s:%d:  %s failed.\n",
		  __FILE__, __LINE__, __func__);
		exit(1);
	}
}

/* C string sorting */

static int cstrcmp(const void* A, const void* B)
{
	const char** a = (const char**)A;
	const char** b = (const char**)B;
	return strcmp(*a, *b);
}

static bool cstrlessthan(const char* a, const char* b)
{
	return strcmp(a, b) < 0;
}

static char* dict = NULL;
static size_t dict_nbytes;

static char** word;
static int nWord;

static void load_dict()
{
	/* Already loaded? */
	if (dict != NULL)
		return;

	/* Load dictionary into memory */
	size_t dict_cap;
	csnip_arr_Init(dict, dict_nbytes, dict_cap, 4096, _);
	const char* wordlist = getenv("WORDLIST");
	if (wordlist == NULL)
		wordlist = "/usr/share/dict/words";
	FILE* fp = fopen(wordlist, "r");
	if (fp == 0) {
		fprintf(stderr, "Error:  Cannot open word list \"%s\"\n",
			wordlist);
		exit(1);
	}
	const size_t bsz = 4096;
	size_t r = 0;
	do {
		csnip_arr_Reserve(dict, dict_nbytes, dict_cap,
			dict_nbytes + bsz, _);
		r = fread(&dict[dict_nbytes], 1, bsz, fp);
		dict_nbytes += r;
	} while(r == bsz);
	fclose(fp);

	/* Create word list */
	int word_cap;
	csnip_arr_Init(word, nWord, word_cap, 1024, _);
	char* p = dict;
	char* q = dict;
	while (q < &dict[dict_nbytes]) {
		if (*q == '\n' || *q == '\0') {
			*q = '\0';
			csnip_arr_Push(word, nWord, word_cap, p, _);
			p = &q[1];
		}
		++q;
	}

	/* Mix up address space.
	 * We want to avoid having the addresses ordered correctly. */
	int* perm = new int[nWord];
	for (int i = 0; i < nWord; ++i) {
		perm[i] = i;
	}
	for (int i = 0; i < nWord - 1; ++i) {
		const int u = i + int(std::rand() /
				(RAND_MAX + 1.0) * (nWord - i));
		csnip_Tswap(int, perm[i], perm[u]);
	}

	char* newdict = new char[dict_nbytes];
	p = newdict;
	for (int i = 0; i < nWord; ++i) {
		strcpy(p, word[perm[i]]);
		word[perm[i]] = p;
		p += strlen(p) + 1;
	}
	delete[] dict;
	dict = newdict;

	/* Put addresses back in order */
	csnip_Qsort(u, v, word[u] < word[v],
		csnip_Tswap(char*, word[u], word[v]),
		nWord);
}

void create_cstr_instance(char** arr, int nItem, task_t task)
{
	load_dict();

	/* Select the words to use */
	switch (task) {
	case T_RANDOM:
	case T_INCREASING:
	case T_DECREASING:
		for (int j = 0; j < nItem; ++j) {
			const int u = int(std::rand() / (RAND_MAX + 1.0)
						* nWord);
			arr[j] = word[u];
		}

		if (task == T_INCREASING) {
			csnip_Qsort(u, v, strcmp(arr[u], arr[v]) < 0,
				csnip_Tswap(char*, arr[u], arr[v]),
				nItem);
		} else if (task == T_DECREASING) {
			csnip_Qsort(u, v, strcmp(arr[u], arr[v]) > 0,
				csnip_Tswap(char*, arr[u], arr[v]),
				nItem);
		}
		break;
	case T_DNF: {
		int p[4];
		for (int i = 0; i < 4; ++i)
			p[i] = int(std::rand() / (RAND_MAX + 1.0)
					* nWord);

		for (int j = 0; j < nItem; ++j) {
			const int u = int(std::rand() / (RAND_MAX + 1.0) * 4);
			arr[j] = word[p[u]];
		}
		break;
	}
	case T_ALLEQ: {
		const int p = int(std::rand() / (RAND_MAX + 1.0) * nWord);
		for (int j = 0; j < nItem; ++j) {
			arr[j] = word[p];
		}
		break;
	}
	case T_ORGANPIPE: {
		int i, j;
		for (i = 0; 2*i <= nItem; ++i) {
			const int u = int(std::rand() / (RAND_MAX + 1.0)
						* nWord);
			arr[i] = word[u];
		}
		csnip_Qsort(u, v, strcmp(arr[u], arr[v]) < 0,
				csnip_Tswap(char*, arr[u], arr[v]),
				i);
		for (j = 0; j < i; ++j) {
			arr[nItem - j - 1] = arr[j];
		}
		break;
	}
	};
}

void sort_cstr_instance(char** arr,
			int nItem,
			sort_method_t meth)
{
	switch (meth) {
	case M_STD_SORT:
		std::sort(arr, &arr[nItem], cstrlessthan);
		break;
	case M_STD_QSORT:
		std::qsort(arr, nItem,
		  sizeof(arr[0]), cstrcmp);
		break;
	case M_CSNIP_QSORT:
		csnip_Qsort(u, v, strcmp(arr[u], arr[v]) < 0,
			csnip_Tswap(char*, arr[u], arr[v]),
			nItem);
		break;
	case M_CSNIP_HEAPSORT:
		csnip_Heapsort(u, v, strcmp(arr[u], arr[v]) < 0,
			csnip_Tswap(char*, arr[u], arr[v]),
			nItem);
		break;
	case M_CSNIP_SHELLSORT:
		csnip_Shellsort(u, v, strcmp(arr[u], arr[v]) < 0,
			csnip_Tswap(char*, arr[u], arr[v]),
			nItem);
		break;
	};
}

void check_cstr_instance(char** strarr, int nItem)
{
	int result;
	csnip_IsSorted(u, v, strcmp(strarr[u], strarr[v]) < 0,
	  nItem, result);
	if (!result) {
		fprintf(stderr, "%s:%d:  %s failed.\n",
		  __FILE__, __LINE__, __func__);
		exit(1);
	}
}

/* Test execution */

void sort_test(int nItem,
		sort_method_t meth,
		task_t task,
		sortkey_t key_type)
{
	/* Allocate instance memory */
	int* intarr = NULL;
	char** strarr = NULL;
	switch(key_type) {
	case K_INT:	intarr = new int[nItem];	break;
	case K_CSTR:	strarr = new char*[nItem];	break;
	};

	/* Execute test runs */
	double t_total = 0;
	int nIter = 0;
	while (t_total < 10)
	{
		/* Create instance to solve */
		switch(key_type) {
		case K_INT:
			create_int_instance(intarr, nItem, task);
			break;
		case K_CSTR:
			create_cstr_instance(strarr, nItem, task);
			break;
		};

		/* Test sort */
		struct timespec t_start, t_end;
		x_clock_gettime(X_CLOCK_MAYBE_MONOTONIC, &t_start);
		switch(key_type) {
		case K_INT:
			sort_int_instance(intarr, nItem, meth);
			break;
		case K_CSTR:
			sort_cstr_instance(strarr, nItem, meth);
			break;
		};
		x_clock_gettime(X_CLOCK_MAYBE_MONOTONIC, &t_end);

		/* Check */
		switch(key_type) {
		case K_INT:
			check_int_instance(intarr, nItem);
			break;
		case K_CSTR:
			check_cstr_instance(strarr, nItem);
			break;
		};

		/* Move up */
		t_total += get_delta(&t_end, &t_start);
		++nIter;

	}

	/* Free memory */
	if (strarr)	delete[] strarr;
	if (intarr)	delete[] intarr;

	/* Output result */
	std::printf("%g s for %d iterations -> %g s per iteration.\n",
		t_total, nIter, t_total/nIter);
}

static void usage()
{
	puts(
	"sorting performance tester.\n"
	"\n"
        "-h             Display help and exit.\n"
        "-N #           Number of items to sort.\n"
        "-m meth        Sort method to use. Possible choices:\n"
        "                 std::sort   (STL algorithm)\n"
        "                 std::qsort  (libc qsort)\n"
        "                 Qsort       (csnip's Quicksort)\n"
        "                 Heapsort    (csnip's Heapsort)\n"
        "                 Shellsort   (csnip's Shellsort)\n"
	"-t task	Sorting task. Possible choices:\n"
	"                 random      (data is in random order)\n"
	"                 inc         (data is increasing)\n"
	"                 dec         (data is decreasing)\n"
	"                 dnf         (data is a random sequence of just 4\n"
	"                              distinct values)\n"
	"                 alleq       (all data values are the same)\n"
	"                 organpipe   (data increasing then decreasing)\n"
	"-k key		Key type. Possible choices:\n"
	"                 int         (integer keys)\n"
	"                 cstr        (C string keys)\n"
	);
}

int main(int argc, char** argv)
{
	sort_method_t meth = M_CSNIP_QSORT;
	task_t task = T_RANDOM;
	sortkey_t key_type = K_INT;
	int nItem = 10000;

	int c;
	while ((c = x_getopt(argc, argv, "k:m:N:t:h")) != -1) {
		switch (c) {
		case 'k': {
			if (strcmp(x_optarg, "int") == 0) {
				key_type = K_INT;
			} else if (strcmp(x_optarg, "cstr") == 0) {
				key_type = K_CSTR;
			}
			break;
		}
		case 'm': {
			struct {
				const char* name;
				sort_method_t m;
			} mtable[] = {
			  { "std::sort",	M_STD_SORT },
			  { "std::qsort",	M_STD_QSORT },
			  { "Qsort",		M_CSNIP_QSORT },
			  { "Heapsort",		M_CSNIP_HEAPSORT },
			  { "Shellsort",	M_CSNIP_SHELLSORT },
			  { NULL }
			};
			int i;
			for (i = 0; mtable[i].name; ++i) {
				if (strcmp(mtable[i].name, x_optarg) == 0) {
					meth = mtable[i].m;
					break;
				}
			}
			if (mtable[i].name == NULL) {
				fprintf(stderr, "error: sort method `%s' "
				 "unknown.\n", x_optarg);
				exit(1);
			}
			break;
		}
		case 'N': {
			nItem = atoi(x_optarg);
			break;
		}
		case 't': {
			struct {
				const char* name;
				task_t task;
			} ttable[] = {
			  { "random",		T_RANDOM },
			  { "inc",		T_INCREASING },
			  { "dec",		T_DECREASING },
			  { "dnf",		T_DNF },
			  { "alleq",		T_ALLEQ },
			  { "organpipe",	T_ORGANPIPE },
			  { NULL }
			};
			int i;
			for (i = 0; ttable[i].name; ++i) {
				if (strcmp(ttable[i].name, x_optarg) == 0) {
					task = ttable[i].task;
					break;
				}
			}
			if (ttable[i].name == NULL) {
				fprintf(stderr, "error: task type `%s' "
				  "unknown.\n", x_optarg);
				exit(1);
			}
			break;
		}
		case 'h':
			usage();
			exit(0);
		};
	}

	/* seed rng */
	std::srand(time(NULL));

	/* Run test */
	sort_test(nItem, meth, task, key_type);

	return 0;
}
