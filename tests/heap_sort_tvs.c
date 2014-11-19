#include "heap_sort_tvs.h"

/* corner case test for an empty array. */
static struct heap_sort_tv empty_array = {
	.n_keys = 0,
};

static int single_element_key[1] = {0};

/* test for just one array element. */
static struct heap_sort_tv single_element = {
	.n_keys = 1,
	.keys = single_element_key,
};

#define N_MULTIPLE	8
static int multiple_elements_keys[N_MULTIPLE] = {1, 2, 4, 8, -1, -4, -2, -8};

/* test for multiple, distinct keys */
static struct heap_sort_tv multiple_elements = {
	.n_keys = N_MULTIPLE,
	.keys = multiple_elements_keys,
};

static int repeat_elements_keys[N_MULTIPLE] = {0, 2, 4, 8, 0, -4, -2, -8};

/* corner case test for multiple keys, but with a repeat */
static struct heap_sort_tv repeat_elements = {
	.n_keys = N_MULTIPLE,
	.keys = repeat_elements_keys,
};

struct heap_sort_tv *heap_sort_tvs[N_HEAP_SORT_TVS] = {
	&empty_array, &single_element, &multiple_elements, &repeat_elements
};
