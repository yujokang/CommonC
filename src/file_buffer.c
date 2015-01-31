#include <file_buffer.h>
#include <logger.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* the page size of the system */
#define PAGE_SIZE	getpagesize()
int init_file_buffer(file_buffer_t *to_init, FILE *in_file)
{
	unsigned char *buffer;
	long file_size;

	/* Find the size of the file, by finding the last location. */
	if (fseek(in_file, 0, SEEK_END)) {
		printlg(ERROR_LEVEL, "Failed to reach the end of the file.\n");
		return -1;
	}

	file_size = ftell(in_file);
	rewind(in_file);
	if (file_size < 0) {
		printlg(ERROR_LEVEL, "Failed to find the size of the file.\n");;
		return -1;
	}

	/* allocate buffer */
	buffer = malloc(PAGE_SIZE);
	if (buffer == NULL) {
		printlg(ERROR_LEVEL, "Failed to allocate file buffer.\n");
		errno = ENOMEM;
		return -1;
	}

	/* Finalize field values. */
	to_init->in_file = in_file;
	to_init->file_size = (size_t) file_size;

	to_init->buffer = buffer;
	to_init->buffered = 0;

	to_init->virtual_position = 0;
	to_init->real_position = 0;

	printlg(DEBUG_LEVEL, "File's size is %u.\n",
		(unsigned) to_init->file_size);
	printlg(DEBUG_LEVEL, "File's buffer is at %p.\n", to_init->buffer);
	printlg(DEBUG_LEVEL, "File's buffer status is %d.\n",
		to_init->buffered);
	printlg(DEBUG_LEVEL, "File's virtual pointer is %u.\n",
		(unsigned) to_init->virtual_position);
	printlg(DEBUG_LEVEL, "File's real pointer is %u.\n",
		(unsigned) to_init->real_position);

	return 0;
}

int open_file_buffer(file_buffer_t *to_open, const char *path)
{
	FILE *in_file = fopen(path, "r");

	if (in_file == NULL) {
		printlg(ERROR_LEVEL, "Failed to open file, %s, for buffer.\n",
			path);
		return -1;
	}

	if (init_file_buffer(to_open, in_file)) {
		fclose(in_file);
		return -1;
	}

	return 0;
}

void destroy_file_buffer(file_buffer_t *to_destroy)
{
	free(to_destroy->buffer);
	to_destroy->buffer = NULL;
	to_destroy->buffered = 0;

	to_destroy->virtual_position = 0;
	to_destroy->real_position = 0;
}

void close_file_buffer(file_buffer_t *to_close)
{
	destroy_file_buffer(to_close);
	fclose(to_close->in_file);
	to_close->in_file = NULL;
}

int fseek_buffer(file_buffer_t *buffer, long offset, int whence)
{
	long dest;

	/* determine location */
	switch (whence) {
	case SEEK_SET:
		dest = offset;
		break;
	case SEEK_CUR:
		dest = buffer->virtual_position + offset;
		break;
	case SEEK_END:
		dest = buffer->file_size + offset;
		break;
	default:
		printlg(ERROR_LEVEL,
			"Invalid value for \"whence\" parameter: %d.\n",
			whence);
		errno = EINVAL;
		return -1;
	}

	/* Check that the location is in range. */
	if (dest < 0 || (size_t) dest > buffer->file_size) {
		printlg(ERROR_LEVEL, "Invalid destination, %ld.\n", dest);
		errno = ERANGE;
		return -1;
	}

	/*
	 * If the new location is outside of the buffered range,
	 * move the real cursor.
	 */
	if ((dest > buffer->real_position) ||
	    (dest < buffer->real_position - PAGE_SIZE)) {
		if (fseek(buffer->in_file, dest, SEEK_SET)) {
			printlg(ERROR_LEVEL,
				"Unable to move file position to cursor.\n");
			return -1;
		}
		buffer->real_position = dest;
		buffer->buffered = 0;
	}

	buffer->virtual_position = dest;

	return 0;
}

void rewind_buffer(file_buffer_t *buffer)
{
	buffer->virtual_position = 0;

	/* Really rewind only if we can't keep the buffered data. */
	if (buffer->real_position > PAGE_SIZE || !buffer->buffered) {
		rewind(buffer->in_file);
		buffer->buffered = 0;

		buffer->real_position = 0;
	}
}

long ftell_buffer(file_buffer_t *buffer)
{
	return buffer->virtual_position;
}

/*
 * Read bytes that are known to be unallocated,
 * and do not extend past the end of the file.
 * ptr:		the destination pointer
 * size:	the number of bytes to read
 * buffer:	the source buffer
 * returns	number of bytes read
 */
static size_t read_new(void *ptr, size_t size, file_buffer_t *buffer)
{
	int
	reached_end = (buffer->virtual_position + size) == buffer->file_size;
	size_t real_size = reached_end ?
			   buffer->file_size - buffer->virtual_position : size;
	size_t n_full_pages = real_size / PAGE_SIZE;
	size_t remainder_bytes = real_size % PAGE_SIZE;
	size_t pages_read, bytes_read;

	debug_assert(buffer->virtual_position + size <= buffer->file_size);

	/* Read the full pages that need to be in the output. */
	pages_read = fread(ptr, PAGE_SIZE, n_full_pages, buffer->in_file);
	bytes_read = pages_read * PAGE_SIZE;
	buffer->virtual_position += bytes_read;
	buffer->real_position += bytes_read;

	if (pages_read < n_full_pages) {
		printlg(ERROR_LEVEL,
			"Failed to read desired number of full pages.\n");
		return bytes_read;
	}

	/*
	 * If there are more bytes, read up to a page or the rest of the file
	 * into the buffer,
	 * and copy only the needed part into the output.
	 * But always record that the real pointer advanced by a full page,
	 * to keep the start of the buffer a page before the real pointer.
	 */
	if (remainder_bytes > 0) {
		size_t rest_to_read = reached_end ?
				      remainder_bytes : (size_t) PAGE_SIZE;

		if (fread(buffer->buffer, rest_to_read, 1,
			  buffer->in_file) == 0) {
			printlg(ERROR_LEVEL,
				"Failed to read last part of desired bytes.\n");
		} else {
			memcpy(ptr + bytes_read, buffer->buffer,
			       remainder_bytes);

			debug_assert(remainder_bytes < (size_t) PAGE_SIZE);

			buffer->buffered = 1;

			buffer->virtual_position += remainder_bytes;
			buffer->real_position += PAGE_SIZE;
			bytes_read += remainder_bytes;
			debug_assert(bytes_read == real_size);
		}
	}
	return bytes_read;
}

size_t read_buffer_bytes(void *ptr, size_t size, file_buffer_t *buffer)
{
	/* First, try to read any buffered bytes. */
	int reached_end = buffer->virtual_position + size > buffer->file_size;
	size_t real_size = reached_end ?
			   buffer->file_size - buffer->virtual_position : size;
	size_t bytes_left = buffer->buffered ?
			    buffer->real_position - buffer->virtual_position :
			    0;
	size_t bytes_read;

	printlg(DEBUG_LEVEL, "Wanted to read %u bytes starting from %u.\n",
		(unsigned) size, (unsigned) buffer->virtual_position);
	printlg(DEBUG_LEVEL,
		"Actually reading %u bytes. The file ends at %u.\n",
		(unsigned) real_size, (unsigned) buffer->file_size);

	if (bytes_left > 0) {
		unsigned char *
		buffer_start = buffer->buffer + PAGE_SIZE - bytes_left;

		if (bytes_left >= real_size) {
			memcpy(ptr, buffer_start, real_size);
			buffer->virtual_position += real_size;
			return real_size;
		} else {
			memcpy(ptr, buffer_start, bytes_left);
			buffer->virtual_position += bytes_left;

			bytes_read = bytes_left;
		}
	} else {
		bytes_read = 0;
	}

	/* Read the remaining bytes from file. */
	buffer->buffered = 0;
	bytes_read += read_new(ptr + bytes_read, real_size - bytes_read,
			       buffer);
	return bytes_read;
}

int fgetc_buffer(file_buffer_t *buffer)
{
	unsigned char byte;

	if (read_buffer_bytes(&byte, 1, buffer) == 0) {
		return EOF;
	}
	return (int) byte;
}
