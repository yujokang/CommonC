#include <get_random.h>
#include <logger.h>

#define RANDOM_FILE "/dev/urandom"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

size_t get_random(void *output, size_t size)
{
	FILE *random_file = fopen(RANDOM_FILE, "r");
	size_t read_bytes;

	if (random_file == NULL) {
		printlg(ERROR_LEVEL,
			"Could not open random source, due to error %d.\n",
			errno);
		return 0;
	}

	read_bytes = fread(output, size, 1, random_file);
	fclose(random_file);

	return read_bytes;
}
