/*
 * Fetch random bytes from system.
 */
#ifndef GET_RANDOM
#define GET_RANDOM

#include <inttypes.h>
#include <stdlib.h>

/*
 * Read random bytes from the system into the output buffer.
 * output:	the output buffer to which to copy the random bytes
 * size:	the desired number of bytes to fetch
 * returns	the number of random bytes successfully read
 */
size_t get_random(void *output, size_t size);

#endif /* GET_RANDOM */
