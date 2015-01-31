/* runs tests on the functions in "file_buffer.h" */
#include "file_buffer_tvs.h"

#include <logger.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/* the directory containing all of the files */
#define TEST_FILE_DIR		"file_buffer_inputs/"
#define TEST_FILE_DIR_LEN	strlen(TEST_FILE_DIR)

/*
 * Generate a memory map of the given file,
 * which will be used for checking the values read from the buffer.
 * fd:		the output pointer for the file descriptor used for the mapping,
 *		so that it can be properly closed afterwards.
 *		Set to -1 on error
 * path:	the path of the file to map
 * file_size:	the size of the file
 * returns	the mapping of the file on success,
 *		NULL on error, with errno set by "open" or "mmap"
 */
static unsigned char *gen_file_map(int *fd, char *path, size_t file_size)
{
	unsigned char *file_map;

	*fd = open(path, O_RDONLY);

	if (*fd < 0) {
		printlg(ERROR_LEVEL, "Failed to open file %s for mapping.\n",
			path);
		return NULL;
	}

	file_map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, *fd, 0);

	if (file_map == MAP_FAILED) {
		close(*fd);
		*fd = -1;

		printlg(ERROR_LEVEL, "Failed to map file to memory.\n");
		return NULL;
	}

	return file_map;
}

/*
 * Run a single file buffer test case.
 * tv:		the file buffer test vector
 * returns	1 if passed, 0 otherwise
 */
static int test_file_buffer(struct file_buffer_tv *tv)
{
	size_t name_len = strlen(tv->file_name) + 1;
	char path[TEST_FILE_DIR_LEN + name_len];
	unsigned char *file_map;
	size_t file_size;
	int fd;
	file_buffer_t test_buffer;
	int passed;

	/* Open the file buffer. */
	memcpy(path, TEST_FILE_DIR, TEST_FILE_DIR_LEN);
	memcpy(path + TEST_FILE_DIR_LEN, tv->file_name, name_len);

	if (open_file_buffer(&test_buffer, path)) {
		printlg(ERROR_LEVEL, "Failed to open test file %s.\n", path);
		return 0;
	}

	debug_assert(ftell_buffer(&test_buffer) == 0);

	/* Map the file, then run the test. */
	file_size = get_file_size(&test_buffer);
	if ((file_map = gen_file_map(&fd, path, file_size)) == NULL) {
		printlg(ERROR_LEVEL, "Failed to map file.\n");
		passed = 0;
	} else {
		passed = tv->tester(&test_buffer, file_map);
		munmap(file_map, file_size);
		close(fd);
	}

	close_file_buffer(&test_buffer);
	return passed;
}

/*
 * Run all of the test cases in "file_buffer_tvs"
 */
static void test_file_buffers()
{
	size_t tv_i;

	for (tv_i = 0; tv_i < N_FILE_BUFFER_TVS; tv_i++) {
		printlg(INFO_LEVEL, "Running file buffer test %u...\n",
			(unsigned) tv_i);
		if ((test_file_buffer(file_buffer_tvs[tv_i]))) {
			printlg(INFO_LEVEL, "Passed!\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n");
		}
	}
}

int main(void)
{
	test_file_buffers();

	return 0;
}
