/* declarations of test buffer testing vectors */
#include <file_buffer.h>

#include <stdlib.h>

/* vector to test file buffer functions */
struct file_buffer_tv {
	/*
	 * the name of the file in the "file_buffer_inputs/" directory to open
	 */
	char *file_name;
	/*
	 * Runs the tests using functions from "file_buffer.h".
	 * buffer:	the opened file buffer
	 * file_map:	the mapping to the file from which to read,
	 *		used to check that the reading was done correctly
	 * returns	1 if passed, 0 otherwise
	 */
	int (*tester)(file_buffer_t *buffer, unsigned char *file_map);
};

#define N_FILE_BUFFER_TVS 6
/* all the test vectors that will be run by "test_file_buffers" */
extern struct file_buffer_tv *file_buffer_tvs[N_FILE_BUFFER_TVS];
