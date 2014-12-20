#include "permutation_tvs.h"

#include <permutation.h>
#include <logger.h>

/*
 * Check that the inverse is correct,
 * ie. applying the inverse to the output of the permutation
 * gives the input of the permutation.
 * permutation:	the original permutation
 * inverse:	the supposed inverse of the permutation
 * size:	the size of the permutation and its inverse
 * returns	1 iff the inverse is correct, 0 otherwise
 */
static int check_inverse(uint32_t *permutation, uint32_t *inverse,
			 uint32_t size)
{
	uint32_t in_i;
	int passed = 1;

	for (in_i = 0; in_i < size; in_i++) {
		uint32_t re_in = inverse[permutation[in_i]];
		if (re_in != in_i) {
			printlg(ERROR_LEVEL,
				"Input %u maps to %u, rather than itself.\n",
				in_i, re_in);
			passed = 0;
		}
	}

	return passed;
}

/*
 * Test the "invert" function on a given permutation.
 * tv:		the test vector containing the permutation to invert
 * returns	1 iff the correct inverse was created. 0 otherwise.
 */
static int test_inverse(struct invert_tv *tv)
{
	uint32_t inverse[tv->size];

	invert(inverse, tv->permutation, tv->size);

	return check_inverse(tv->permutation, inverse, tv->size);
}

/*
 * Test inversion of all the permutations in "invert_tvs"
 */
static void test_inverses()
{
	size_t test_i;

	for (test_i = 0; test_i < N_INVERT_TVS; test_i++) {
		printlg(INFO_LEVEL, "Inverse test %u...\n", (unsigned) test_i);
		if (test_inverse(invert_tvs[test_i])) {
			printlg(INFO_LEVEL, "Passed!\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n");
		}
	}
}

/* the size of the random permutation in "test_random_inverse" */
#define RANDOM_INVERSE_SIZE	10
/*
 * Generate a random permutation, print it out,
 * invert it, and check the inverse.
 */
static void test_random_inverse()
{
	uint32_t permutation[RANDOM_INVERSE_SIZE], inverse[RANDOM_INVERSE_SIZE];
	uint32_t in_i;

	if (permute(permutation, RANDOM_INVERSE_SIZE)) {
		printlg(ERROR_LEVEL,
			"Failed to generate random permutation to test.\n");
		return;
	}

	printlg(INFO_LEVEL, "Testing inverse of permutation:\n");
	for (in_i = 0; in_i < RANDOM_INVERSE_SIZE; in_i++) {
		printlg(INFO_LEVEL, "\t%u\n", permutation[in_i]);
	}

	invert(inverse, permutation, RANDOM_INVERSE_SIZE);

	if (check_inverse(permutation, inverse, RANDOM_INVERSE_SIZE)) {
		printlg(INFO_LEVEL, "Passed!\n");
	} else {
		printlg(ERROR_LEVEL, "Failed!\n");
	}
}

int main(void)
{
	test_inverses();
	test_random_inverse();
	return 0;
}
