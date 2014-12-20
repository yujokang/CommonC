/*
 * Functions for calculations involving overflows
 * and large numbers divided into halves.
 */
#ifndef XMATH_H
#define XMATH_H

#include <inttypes.h>

/*
 * the slow, but portable method of multiplying 64-bit integers,
 * and finding both the old result and the overflow
 * out_high:	the high half of the output, ie. the overflow
 * out_low:	the low half of the output, which is also the result of the
 *		actual multiplication operation
 * in_0:	the first factor
 * in_1:	the second factor
 */
void xmultiply_slow(uint64_t *out_high, uint64_t *out_low,
		    uint64_t in_0, uint64_t in_1);

/*
 * Find both the old result and the overflow
 * of the multiplication of two 64-bit words.
 * out_high:	the high half of the output, ie. the overflow
 * out_low:	the low half of the output, which is also the result of the
 *		actual multiplication operation
 * in_0:	the first factor
 * in_1:	the second factor
 */
inline static void
xmultiply(uint64_t *out_high, uint64_t *out_low, uint64_t in_0, uint64_t in_1)
{
#if defined(__amd64__) || defined(__amd) || \
    defined(__x86_64__) || defined(__x86_64) || \
    defined(_M_X64) || defined(_M_AMD64)
	/*
	 * For x86-64, use the 64-bit multiplication instruction,
	 * in which both outputs are stored in registers.
	 */
	asm("movq %2, %%rax\n\t"
	    "mulq %3\n\t"
	    "movq %%rdx, %0\n\t"
	    "movq %%rax, %1\n\t"
	    : "=m" (*out_high), "=m" (*out_low)
	    : "r" (in_0), "r" (in_1)
	    : "%rdx", "%rax");
#else
	/*
	 * Otherwise, use the generic method.
	 */
	xmultiply_slow(out_high, out_low, in_0, in_1);
#endif
}

/*
 * Calculate the modulo of a 128-bit integer, represented by 2 64-bit integers.
 * high:	the high half of the 128-bit integer
 * low:		the low half of the 128-bit integer
 * mod:		the modulus to calculate
 * returns	the remainder of the modulo operation
 */
uint64_t xmod(uint64_t high, uint64_t low, uint64_t mod);

#endif /* XMATH_H */
