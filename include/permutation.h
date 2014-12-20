/*
 * function to randomly generate permutation
 */
#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <stdlib.h>
#include <inttypes.h>

/*
 * Generate a random permutation of elements.
 * The statistical distance between the permutation and
 * a permutation chosen uniformly at random is at most
 * ((size) (size + 1) / 2 ^ 129), which is negligible, since size < 2 ^ 32.
 * permute_output:	the output space for the permutation
 * size:		the number of elements to permute
 * returns		0 on success, -1 on error when drawing random bytes
 */
int permute(uint32_t *permute_output, uint32_t size);

/*
 * Calculate inverse of a permutation.
 * inverse:	the inverse to calculate
 * permutation:	the permutation to invert
 * size:	the number of elements to permute
 */
void invert(uint32_t *inverse, uint32_t *permutation, uint32_t size);

#endif /* PERMUTATION_H */
