#include "xmath_tvs.h"

#include <xmath.h>
#include <logger.h>

/*
 * Generic multiplication test for both methods
 * tv:		the test vector containing the factors and expected product
 * multiplier:	the multiplication method
 * returns	1 iff successful, 0 otherwise
 */
static int
_test_xmultiply(struct xmultiply_tv *tv,
		void (*multiplier)(uint64_t *out_high, uint64_t *out_low,
				   uint64_t in_0, uint64_t in_1))
{
	uint64_t out_high, out_low;
	int passed = 1;

	multiplier(&out_high, &out_low, tv->in_0, tv->in_1);

	if (out_high != tv->out_high) {
		printlg(ERROR_LEVEL,
			"High half of multiplication does not match. "
			"Expected %lx, but got %lx.\n",
			tv->out_high, out_high);
		passed = 0;
	}
	if (out_low != tv->out_low) {
		printlg(ERROR_LEVEL,
			"Low half of multiplication does not match. "
			"Expected %lx, but got %lx.\n",
			tv->out_low, out_low);
		passed = 0;
	}

	return passed;
}

/*
 * Run all the test vectors in "xmultiply_tvs" using the "xmultiply" function.
 */
static void test_xmultiplies()
{
	size_t test_i;

	for (test_i = 0; test_i < N_XMULTIPLY_TVS; test_i++) {
		printlg(INFO_LEVEL, "xmultiply test %u...\n",
			(unsigned) test_i);
		if (_test_xmultiply(xmultiply_tvs[test_i], xmultiply)) {
			printlg(INFO_LEVEL, "Passed!\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n");
		}
	}
}

/*
 * Run all the test vectors in "xmultiply_tvs"
 * using the "xmultiply_slow" function.
 */
static void test_xmultiply_slows()
{
	size_t test_i;

	for (test_i = 0; test_i < N_XMULTIPLY_TVS; test_i++) {
		printlg(INFO_LEVEL, "xmultiply_slow test %u...\n",
			(unsigned) test_i);
		if (_test_xmultiply(xmultiply_tvs[test_i], xmultiply_slow)) {
			printlg(INFO_LEVEL, "Passed!\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n");
		}
	}
}

/*
 * Run a test for the "xmod" function.
 * tv:		the test vector containing the dividend halves,
 *		the divisor, and the expected remainder
 * returns	1 iff successful, 0 otherwise
 */
static int test_xmod(struct xmod_tv *tv)
{
	uint64_t result = xmod(tv->high, tv->low, tv->mod);

	if (result != tv->result) {
		printlg(ERROR_LEVEL,
			"Mod result does not match. "
			"Expected %lx, but got %lx.\n",
			tv->result, result);
		return 0;
	}
	return 1;
}

/*
 * Run all the test vectors in "xmod_tvs".
 */
static void test_xmods()
{
	size_t test_i;

	for (test_i = 0; test_i < N_XMOD_TVS; test_i++) {
		printlg(INFO_LEVEL, "xmod test %u...\n",
			(unsigned) test_i);
		if (test_xmod(xmod_tvs[test_i])) {
			printlg(INFO_LEVEL, "Passed!\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n");
		}
	}
}

int main(void)
{
	test_xmultiplies();
	test_xmultiply_slows();
	test_xmods();
	return 0;
}
