#include "permutation_tvs.h"

/* a single-element permutation */
static uint32_t single_permutation[1] = {
	0
};

static struct invert_tv single = {
	.permutation = single_permutation,
	.size = 1
};

/* an even-sized permutation */
#define EVEN_SIZE	4
static uint32_t even_permutation[EVEN_SIZE] = {
	3, 0, 1, 2
};

static struct invert_tv even = {
	.permutation = even_permutation,
	.size = EVEN_SIZE
};

/* an odd-sized permutation */
#define ODD_SIZE	5
static uint32_t odd_permutation[ODD_SIZE] = {
	3, 0, 1, 4, 2
};

static struct invert_tv odd = {
	.permutation = odd_permutation,
	.size = ODD_SIZE
};

struct invert_tv *invert_tvs[N_INVERT_TVS] = {
	&single, &even, &odd
};
