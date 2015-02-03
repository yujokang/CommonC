#include <xmath.h>
#include <debug_assert.h>

#include <stdlib.h>

/*
 * the number of bits to shift,
 * so that only the high half appears, and is in the lower half
 */
#define SHIFT_HIGH_HALF	32
/* the mask to only show the lower half */
#define MASK_LOW_HALF	0xffffffff

/* a helper data structure for storing the two halves of a 64-bit word */
struct vector_64 {
	/* the halves of the 64-bit word, with the upper 32 bits set to 0 */
	uint64_t high, low;
};

/*
 * Split a 64-bit number into its 32-bit halves
 * vector:	will hold the two halves
 * whole:	the number to split
 */
inline static void split_64(struct vector_64 *vector, uint64_t whole)
{
	vector->low = whole & MASK_LOW_HALF;
	vector->high = whole >> SHIFT_HIGH_HALF;
}

/*
 * Check that a 64-bit number only represents 32 bits,
 * which is required in the fields of "struct vector_64"
 * digit:	the 64-bit integer that should not have the upper 32 bits set
 * returns	1 iff the upper 32 bits are not set, 0 otherwise
 */
inline static int is_pure_digit(uint64_t digit)
{
	return (digit >> SHIFT_HIGH_HALF) == 0;
}

/*
 * Helper function to "join_64"
 * Convert 2 64-bit integers representing 32-bit numbers into one 64-bit number.
 * high:	the high half
 * low:		the low half
 * returns	the 64-bit combination of the 32-bit halves in the vector
 */
inline static uint64_t _join_64(uint64_t high, uint64_t low)
{
	debug_assert(is_pure_digit(high));
	debug_assert(is_pure_digit(low));

	return (high << SHIFT_HIGH_HALF) | low;
}

/*
 * the inverse of "split_64"
 * Convert a vector struct into a single, 64-bit number.
 * vector:	the vector, whose parts to combine into a single number
 * returns	the 64-bit combination of the 32-bit halves in the vector
 */
inline static uint64_t join_64(struct vector_64 *vector)
{
	debug_assert(is_pure_digit(vector->low));
	debug_assert(is_pure_digit(vector->high));

	return (vector->high << SHIFT_HIGH_HALF) | vector->low;
}

/*
 * Add a single, 32-bit number to a vector.
 * vector:	the array of digits, each representing 32 bits,
 *		to which to add, in little-endian order
 * digit:	the 64-bit integer representing a 32-bit digit to add
 * start:	the position to which to add "digit"
 * end:		the last digit to which the function can add
 * returns	1 iff there was an overflow. 0 otherwise
 */
static int add_digit(uint64_t *vector, uint64_t digit, size_t start, size_t end)
{
	uint64_t carry = digit;
	size_t digit_i;

	debug_assert(is_pure_digit(digit));

	for (digit_i = start; carry != 0 && digit_i < end; digit_i++) {
		uint64_t digit_result = vector[digit_i] + carry;
		struct vector_64 digit_result_vector;

		debug_assert(is_pure_digit(carry));

		split_64(&digit_result_vector, digit_result);
		vector[digit_i] = digit_result_vector.low;
		carry = digit_result_vector.high;
	}
	return carry != 0;
}

/*
 * To make iterations easier,
 * fetch half of "struct vector_64" corresponding to integer index
 * in little-endian order.
 * vector:	the vector from which to fetch a half
 * index:	the index representing which half to fetch;
 *		0 for the low half, 1 for the high half
 * returns	the corresponding half of the vector
 */
static inline uint64_t get_digit(struct vector_64 *vector, size_t index)
{
	debug_assert(index < 2);

	return index == 0 ? vector->low : vector->high;
}

/*
 * the maximum number of 32-bit words needed
 * to represent the product of 2 64-bit numbers 
 */
#define OUT_VECTOR_SIZE (2 * 2)

void xmultiply_slow(uint64_t *out_high, uint64_t *out_low,
		    uint64_t in_0, uint64_t in_1)
{
	struct vector_64 in_0_vector, in_1_vector;
	uint64_t out_vector[OUT_VECTOR_SIZE];
	size_t out_i, in_0_i, in_1_i;

	split_64(&in_0_vector, in_0);
	split_64(&in_1_vector, in_1);

	/* Initialize the output to 0. */
	for (out_i = 0; out_i < OUT_VECTOR_SIZE; out_i++) {
		out_vector[out_i] = 0;
	}

	/*
	 * For each pair of halves between in_0 and in_1,
	 * find the product and add the two resulting halves to the output.
	 */
	for (in_0_i = 0; in_0_i < 2; in_0_i++) {
		for (in_1_i = 0; in_1_i < 2; in_1_i++) {
			uint64_t
			digit_result = get_digit(&in_0_vector, in_0_i) *
				       get_digit(&in_1_vector, in_1_i);
			size_t target_base = in_0_i + in_1_i;
			struct vector_64 digit_result_vector;
			int carried;

			split_64(&digit_result_vector, digit_result);

			carried = add_digit(out_vector,
					    digit_result_vector.low,
					    target_base, OUT_VECTOR_SIZE);
			if (!carried) {
				debug_assert(!carried);
			}
			carried = add_digit(out_vector,
					    digit_result_vector.high,
					    target_base + 1, OUT_VECTOR_SIZE);
			debug_assert(!carried);
		}
	}

	*out_low = _join_64(out_vector[1], out_vector[0]);
	*out_high = _join_64(out_vector[3], out_vector[2]);

	debug_assert(*out_low == in_0 * in_1);
}

/* the largest 64-bit integer */
#define MAX_U64	(~((uint64_t) 0))

uint64_t xmod(uint64_t high, uint64_t low, uint64_t mod)
{
	uint64_t high_mod = high, low_mod = low;
	/*
	 * Calculate "(1 << 64) % mod" using the fact that
	 * (a + b) mod c = (a mod c) + (b mod c)
	 */
	uint64_t high_factor = (MAX_U64 % mod) + 1;
	/* but have to make sure that the 0 <= high_factor < mod */
	if (high_factor == mod) {
		high_factor = 0;
	}

	/* keep on reducing the high half until it disappears */
	while (high_mod > 0) {
		uint64_t digit_high_mod = high_mod % mod,
			 digit_low_mod = low_mod % mod;
		uint64_t high_mod_high, high_mod_low;

		xmultiply(&high_mod_high, &high_mod_low,
			  digit_high_mod, high_factor);

		high_mod = high_mod_high;
		low_mod = (digit_low_mod + high_mod_low) % mod;

		/* Was there an overflow in the lower half? */
		if (high_mod_low > MAX_U64 - digit_low_mod) {
			high_mod++;
		}
	}

	return low_mod;
}
