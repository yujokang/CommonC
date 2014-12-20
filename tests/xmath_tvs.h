/*
 * Test vectors for testing "xmultiply_slow", "xmultiply" and "xmod"
 */
#include <inttypes.h>

/*
 * test vector for both "xmultiply_slow" and "xmultiply"
 */
struct xmultiply_tv {
	/* the input factors */
	uint64_t in_0, in_1;
	/* the expected outputs */
	uint64_t out_high, out_low;
};

/*
 * test vector for "xmod"
 */
struct xmod_tv {
	/* the high and low inputs */
	uint64_t high, low;
	/* the divisor */
	uint64_t mod;
	/* the expected remainder */
	uint64_t result;
};

/*
 * all the test vectors that will be run by
 * "test_xmultiplies" and "test_xmultiply_slows" in "test_xmath.c".
 */
#define N_XMULTIPLY_TVS	7
extern struct xmultiply_tv *xmultiply_tvs[N_XMULTIPLY_TVS];
/*
 * all the test vectors that will be run by
 * "test_xmods" in "test_xmath.c".
 */
#define N_XMOD_TVS	5
extern struct xmod_tv *xmod_tvs[N_XMOD_TVS];
