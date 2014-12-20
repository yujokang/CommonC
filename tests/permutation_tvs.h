/*
 * test vectors for testing the "invert" function
 */

#include <inttypes.h>

/*
 * the test vector for the "invert" function.
 * The test vector only contains the permutation information,
 * since the inverse will be checked programatically.
 */
struct invert_tv {
	/* the permutation to invert, ie. the second parameter to "invert" */
	uint32_t *permutation;
	/* the size of the permutation, ie. the third parameter to "invert" */
	uint32_t size;
};

#define N_INVERT_TVS	3
extern struct invert_tv *invert_tvs[N_INVERT_TVS];
