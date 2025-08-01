#include <stdio.h>

#define CSNIP_SHORT_NAMES
#include <csnip/mem.h>
#include <csnip/util.h>

static int align_test()
{
	int sizes[] = { 0, 1, 10, 100, 1000, 10000, 100000 };
	int aligns[] =  { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	for (int i = 0; i < Static_len(sizes); ++i) {
		for (int j = 0; j < Static_len(aligns); ++j) {
			const int size = sizes[i];
			const int align = aligns[j]; 
			printf("Looking at size=%d, align=%d\n", size, align);

			int err = 0;
			char* p;
			mem_AlignedAlloc(size, align, p, err);
			if (err != 0) {
				fprintf(stderr, "Alloc error: "
					"size=%d, align=%d: %d\n",
					sizes[i], aligns[j], err);
				return -1;
			}

			Fill_n(p, sizes[i], 1);

			int cnt = 0;
			for (int e = 0; e < size; ++e) {
				cnt += p[e];
			}
			if (cnt != size) {
				fprintf(stderr, "Verification failed!\n");
				return -1;
			}
			mem_AlignedFree(p);
		}
	}

	return 0;
}

int main(int argc, char** argv)
{

	if (align_test() != 0)
		return 1;
	return 0;
}
