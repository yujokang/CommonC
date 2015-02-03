#include <permutation.h>
#include <get_random.h>
#include <xmath.h>
#include <debug_assert.h>
#include <logger.h>

#include <string.h>

#ifdef DEBUG
/*
 * For debugging, check that the array is actually a permutation.
 * permutation:	the candidate permutation to check
 * size:	the permutation size
 * returns	1 iff the array is a permutation, 0 otherwise
 */
static int is_permutation(uint32_t *permutation, uint32_t size)
{
	uint32_t appearances[size];
	int appeared[size];
	uint32_t out_i, in_i;
	int passed = 1;

	memset(appeared, 0, sizeof(appeared));

	for (in_i = 0; in_i < size; in_i++) {
		out_i = permutation[in_i];
		if (appeared[out_i]) {
			printlg(ASSERT_LEVEL,
				"Output %u appeared for inputs %u and %u.\n",
				out_i, in_i, appearances[out_i]);
			passed = 0;
		} else {
			appearances[out_i] = in_i;
			appeared[out_i] = 1;
		}
	}
	for (out_i = 0; out_i < size; out_i++) {
		if (!appeared[out_i]) {
			printlg(ASSERT_LEVEL, "Output %u did not appear.\n",
				out_i);
			passed = 0;
		}
	}

	return passed;
}
#endif /* DEBUG */

int permute(uint32_t *permute_output, uint32_t size)
{
	uint64_t random_order[size - 1][2];
	uint32_t position_i;

	for (position_i = 0; position_i < size; position_i++) {
		permute_output[position_i] = position_i;
	}

	if (get_random(random_order, sizeof(random_order)) == 0) {
		printlg(ERROR_LEVEL,
			"Could not get random bytes for the permutation.\n");
		return -1;
	}

	for (position_i = 0; position_i < size - 1; position_i++) {
		uint32_t n_choices = size - position_i;
		uint32_t choice, choice_i;
		uint32_t temp;

		choice = xmod(random_order[position_i][0],
			      random_order[position_i][1], n_choices);
		choice_i = position_i + choice;

		temp = permute_output[position_i];
		permute_output[position_i] = permute_output[choice_i];
		permute_output[choice_i] = temp;
	}

	debug_assert(is_permutation(permute_output, size));

	return 0;
}

void invert(uint32_t *inverse, uint32_t *permutation, uint32_t size)
{
	uint32_t in_i;

	debug_assert(is_permutation(permutation, size));

	for (in_i = 0; in_i < size; in_i++) {
		uint32_t out_i = permutation[in_i];
		inverse[out_i] = in_i;
	}

	debug_assert(is_permutation(inverse, size));
}
