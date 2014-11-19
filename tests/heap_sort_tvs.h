/*
 * Declarations of data structure testing vectors.
 */
#include <stdlib.h>

/*
 * vector to test "heap_sort"
 */
struct heap_sort_tv {
	/* the number of keys to test */
	size_t n_keys;
	/* the not-necessarily sorted or distinct keys to test */
	int *keys;
};

#define N_HEAP_SORT_TVS	4
/* all the test vectors that will be run by "test_heap_sorts" */
extern struct heap_sort_tv *heap_sort_tvs[N_HEAP_SORT_TVS];
