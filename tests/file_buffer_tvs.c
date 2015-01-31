#include "file_buffer_tvs.h"

#include <logger.h>
#include <debug_assert.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

/*
 * Check that two, not-necessarily zero-terminated, strings are equal.
 * If there is a difference, print the first location in which the bytes differ,
 * and the different values present in it.
 * expected:	the expected string
 * real:	the actual string
 * len:		the number of bytes to compare
 * returns	1 if strings are equal, 0 otherwise
 */
static int check_string(unsigned char *expected, unsigned char *real,
			size_t len)
{
	size_t char_i;

	if (memcmp(expected, real, len) == 0) {
		return 1;
	}

	for (char_i = 0; char_i < len; char_i++) {
		if (expected[char_i] != real[char_i]) {
			printlg(ERROR_LEVEL,
				"Difference in strings starts from %u, "
				"where %x was expected, "
				"but %x was read.\n",
				(unsigned) char_i,
				expected[char_i], real[char_i]);
			break;
		}
	}
	debug_assert(char_i < len);

	return 0;
}

/*
 * Attempt a read, using "read_buffer_bytes",
 * and check that the length and value are as expected.
 * to_read:		the source buffer from which to read
 * file_map:		the mapping of the file, containing the actual values
 * expected_len:	the expected number of bytes to be read
 * try_len:		the number of bytes that will be attempted to be read
 * returns		1 if the read is as expected, 0 otherwise
 */
static int read_check(file_buffer_t *to_read, unsigned char *file_map,
		      size_t expected_len, size_t try_len)
{
	unsigned char *real = malloc(try_len);
	long start;
	size_t real_len;
	int passed;

	if (real == NULL) {
		printlg(ERROR_LEVEL, "Could not allocate output buffer.\n");
		return 0;
	}

	passed = 0;

	/* Attempt the read, and check the number of bytes actually read. */
	start = ftell_buffer(to_read);
	if ((real_len = read_buffer_bytes(real, try_len, to_read)) !=
	    expected_len) {
		printlg(ERROR_LEVEL, "Expected %u bytes, but got %u.\n",
			(unsigned) expected_len, (unsigned) real_len);
	} else {
		/*
		 * Check that the cursor advanced
		 * by the correct number of bytes.
		 */
		long end = ftell_buffer(to_read);

		if (end < start) {
			printlg(ERROR_LEVEL,
				"Cursor started at %ld, "
				"but ended before it, at %ld.\n",
				start, end);
		} else {
			size_t advancement = (size_t) (end - start);

			if (real_len != advancement) {
				printlg(ERROR_LEVEL,
					"Read %u bytes, "
					"but advanced by %u.\n",
					(unsigned) real_len,
					(unsigned) advancement);
			} else {
				/* Check that the read string is correct. */
				if (check_string(file_map + start, real,
						 real_len)) {
					passed = 1;
				}
			}
		}
	}

	free(real);
	return passed;
}

/* the page size, which will determine most of the reading sizes */
#define PAGE_SIZE	getpagesize()

/* the name of the large file, and its size */
#define LARGE_FILE	"large"
#define LARGE_SIZE	0x10007

static int full_read_tester(file_buffer_t *buffer, unsigned char *file_map)
{
	return read_check(buffer, file_map, LARGE_SIZE, LARGE_SIZE);
}

/* Read the whole file at once. */
static struct file_buffer_tv full_read = {
	.file_name = LARGE_FILE,
	.tester = full_read_tester
};

/* a segment that is smaller than a page */
#define SMALL_SEGMENT	(PAGE_SIZE / 2)
/* a segment that is larger than a page */
#define LARGE_SEGMENT	(2 * PAGE_SIZE + 3)

/*
 * Read and check a single segment.
 * buffer:		the buffer from which to read
 * file_map:		the mapping containing the expected value of the segment
 * reading_small:	Will the function read "SMALL_SEGMENT" bytes (1),
 *			or "LARGE_SEGMENT" bytes (0)?
 * bytes_read:		the number of bytes already read
 * returns		the number of bytes read (which should never be 0),
 *			or 0 on failure
 */
static size_t test_segment(file_buffer_t *buffer, unsigned char *file_map,
			   int reading_small, size_t bytes_read)
{
	size_t segment_size = reading_small ? SMALL_SEGMENT: LARGE_SEGMENT;
	size_t bytes_left = LARGE_SIZE - bytes_read;
	size_t bytes_expected = bytes_left < segment_size ?
				bytes_left : segment_size;

	debug_assert(bytes_left > 0);
	debug_assert(bytes_expected > 0);

	if (read_check(buffer, file_map, bytes_expected, segment_size)) {
		return bytes_expected;
	}

	return 0;
}

static int
segmented_read_tester(file_buffer_t *buffer, unsigned char *file_map)
{
	size_t bytes_read = 0;
	int reading_small = 0;

	debug_assert(LARGE_SIZE > LARGE_SEGMENT);
	while (bytes_read < LARGE_SIZE) {
		size_t segment_bytes_read = test_segment(buffer,
							 file_map,
							 reading_small,
							 bytes_read);

		if (segment_bytes_read == 0) {
			printlg(ERROR_LEVEL,
				"Failed reading segment at "
				"%u bytes into the file.\n",
				(unsigned) bytes_read);
			return 0;
		}
		bytes_read += segment_bytes_read;
		reading_small = !reading_small;
	}

	return 1;
}

/* Alternate reading small and large files until the end of the file. */
static struct file_buffer_tv segmented_read = {
	.file_name = LARGE_FILE,
	.tester = segmented_read_tester
};

/* a file that should be smaller than a page, and its size */
#define SMALL_FILE	"small"
#define SMALL_SIZE	0x7

static int small_read_tester(file_buffer_t *buffer, unsigned char *file_map)
{
	return read_check(buffer, file_map, SMALL_SIZE, PAGE_SIZE);
}

/* Try to read a page from the small file. */
static struct file_buffer_tv small_read = {
	.file_name = SMALL_FILE,
	.tester = small_read_tester
};

/*
 * Check that the virtual pointer of the file is in the correct location.
 * buffer:		the buffer whose virtual pointer to check
 * expected_location:	the correct location of the virtual pointer
 * returns		1 if the real location is correct, 0 otherwise
 */
static int check_location(file_buffer_t *buffer, long expected_location)
{
	long real_location;

	real_location = ftell_buffer(buffer);

	if (expected_location != real_location) {
		printlg(ERROR_LEVEL,
			"Expected location %ld, but at %ld.\n",
			expected_location, real_location);
		return 0;
	}

	return 1;
}

/*
 * Rewind the buffer, and check that the virtual pointer is at 0.
 * buffer:	the buffer to rewind
 * returns	1 if the rewind was successful, 0 otherwise
 */
static int check_rewind(file_buffer_t *buffer)
{
	rewind_buffer(buffer);

	if (!check_location(buffer, 0)) {
		printlg(ERROR_LEVEL, "Failed to rewind.\n");
		return 0;
	}

	return 1;
}

static int
jumping_read_tester(file_buffer_t *buffer, unsigned char *file_map)
{
	/* Jump to the end, and don't expect to read any bytes. */
	if (fseek_buffer(buffer, 0, SEEK_END)) {
		printlg(ERROR_LEVEL, "Failed to jump to end of file.\n");
		return 0;
	}
	if (!check_location(buffer, LARGE_SIZE)) {
		printlg(ERROR_LEVEL, "Failed to reach end of file.\n");
		return 0;
	}
	if (fgetc_buffer(buffer) != EOF) {
		printlg(ERROR_LEVEL,
			"Did not expect to "
			"read a byte at the end of the file.\n");
		return 0;
	}

	/* Read a page, and then read some of the earlier bytes. */
	if (!check_rewind(buffer)) {
		printlg(ERROR_LEVEL, "Failed to rewind from end.\n");
		return 0;
	}
	debug_assert(LARGE_SIZE > PAGE_SIZE);
	if (!read_check(buffer, file_map, PAGE_SIZE, PAGE_SIZE)) {
		printlg(ERROR_LEVEL,
			"Failed to read first page.\n");
		return 0;
	}
#define PAGE_BACK_JUMP	(-PAGE_SIZE * 3 / 4)
	if (fseek_buffer(buffer, -PAGE_BACK_JUMP, SEEK_CUR)) {
		printlg(ERROR_LEVEL,
			"Failed to jump into first page.\n");
		return 0;
	}
	if (!check_location(buffer, PAGE_SIZE - PAGE_BACK_JUMP)) {
		printlg(ERROR_LEVEL,
			"Failed to jump into first page.\n");
		return 0;
	}
	if (!read_check(buffer, file_map, SMALL_SEGMENT, SMALL_SEGMENT)) {
		printlg(ERROR_LEVEL, "Failed to read inside first page.\n");
		return 0;
	}

	/* Rewind, and read part of the first page. */
	if (!check_rewind(buffer)) {
		printlg(ERROR_LEVEL, "Failed to rewind from inside page.\n");
		return 0;
	}
	if (!read_check(buffer, file_map, SMALL_SEGMENT, SMALL_SEGMENT)) {
		printlg(ERROR_LEVEL, "Failed to read beginning.\n");
		return 0;
	}

	/* Read after the first page, and read beginning again. */
	debug_assert(LARGE_SIZE > LARGE_SEGMENT + SMALL_SEGMENT);
	if (fseek_buffer(buffer, LARGE_SEGMENT, SEEK_SET)) {
		printlg(ERROR_LEVEL, "Failed to jump past first page.\n");
		return 0;
	}
	if (!check_location(buffer, LARGE_SEGMENT)) {
		printlg(ERROR_LEVEL,
			"Failed to reach location past first page.\n");
		return 0;
	}
	if (!read_check(buffer, file_map, SMALL_SEGMENT, SMALL_SEGMENT)) {
		printlg(ERROR_LEVEL, "Failed to read after first page.\n");
		return 0;
	}
	if (!check_rewind(buffer)) {
		printlg(ERROR_LEVEL, "Failed to rewind to first page.\n");
		return 0;
	}
	if (!read_check(buffer, file_map, SMALL_SEGMENT, SMALL_SEGMENT)) {
		printlg(ERROR_LEVEL,
			"Failed to read first page "
			"after jumping back to it.\n");
		return 0;
	}


	return 1;
}

/* Read while jumping back and forth inside a file. */
static struct file_buffer_tv jumping_read = {
	.file_name = LARGE_FILE,
	.tester = jumping_read_tester
};

/* the number of legal seeking modes */
#define N_SEEK_MODES	3
/*
 * helper function for automatically finding an invalid seeking mode
 * returns	a seeking mode that is not one of the 3 legal ones
 */
static int find_bad_mode()
{
#ifdef DEBUG
	size_t n_tries = 0;
#endif /* DEBUG */
	int try = INT_MIN;

	while (try <= INT_MAX) {
		if (try != SEEK_SET && try != SEEK_CUR && try != SEEK_END) {
			return try;
		}

		try++;

#ifdef DEBUG
		n_tries++;
		debug_assert(n_tries < N_SEEK_MODES);
#endif /* DEBUG */
	}

	return try;
}

/*
 * Test that an error in "fseek_buffer" is properly caught,
 * and does not change the state of the buffer.
 * buffer:		the buffer in which to make an invalid seek
 * offset:		the "offset" argument to "fseek_buffer"
 * whence:		the "whence" argument to "fseek_buffer"
 * expected_errno:	the expected value of "errno"
 * returns		1 if the error was properly handled, 0 otherwise
 */
static int test_fseek_error(file_buffer_t *buffer, long offset, int whence,
			    int expected_errno)
{
	long old_location = ftell_buffer(buffer);

	if (fseek_buffer(buffer, offset, whence) == 0) {
		printlg(ERROR_LEVEL, "Did not catch fseek error.\n");
		return 0;
	}

	if (expected_errno != errno) {
		printlg(ERROR_LEVEL,
			"Expected error %d for bad destination, "
			"but got %d.\n",
			expected_errno, errno);
		return 0;
	}

	if (!check_location(buffer, old_location)) {
		printlg(ERROR_LEVEL,
			"Location moved, in spite of failed seek.\n");
		return 0;
	}

	return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static int error_read_tester(file_buffer_t *buffer, unsigned char *file_map)
{
	int bad_mode = find_bad_mode();
	if (!test_fseek_error(buffer, 0, bad_mode, EINVAL)) {
		printlg(ERROR_LEVEL, "Did not catch bad mode, %d.\n", bad_mode);
		return 0;
	}

	if (!test_fseek_error(buffer, SMALL_SIZE + 1, SEEK_SET, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"directly jumping past end of file.\n");
		return 0;
	}

	if (!test_fseek_error(buffer, -1, SEEK_SET, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"directly jumping to a negative value.\n");
		return 0;
	}

	if (fseek_buffer(buffer, SMALL_SIZE / 2, SEEK_SET)) {
		printlg(ERROR_LEVEL,
			"Failed to jump to the middle of the file.\n");
		return 0;
	}
	if (!test_fseek_error(buffer, SMALL_SIZE, SEEK_CUR, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"making relative jump past end of file.\n");
		return 0;
	}
	if (!test_fseek_error(buffer, -SMALL_SIZE, SEEK_CUR, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"making relative jump to a negative destination.\n");
		return 0;
	}


	if (!test_fseek_error(buffer, 1, SEEK_END, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"jumping past end of file.\n");
		return 0;
	}
	if (!test_fseek_error(buffer, -(SMALL_SIZE + 1), SEEK_END, ERANGE)) {
		printlg(ERROR_LEVEL,
			"Did not detect error when "
			"jumping past beginning of file.\n");
		return 0;
	}

	return 1;
}
#pragma GCC diagnostic error "-Wunused-parameter"

/* Check that invalid calls to "fseek_buffer" are properly reported. */
static struct file_buffer_tv error_read = {
	.file_name = SMALL_FILE,
	.tester = error_read_tester
};

struct file_buffer_tv *file_buffer_tvs[N_FILE_BUFFER_TVS] = {
	&full_read, &segmented_read, &small_read, &jumping_read, &error_read
};
