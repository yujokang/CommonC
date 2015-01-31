/*
 * buffered wrapper around an input file stream
 * Functions correspond to actual file stream reading operations,
 * but up to a page of file data is buffered in user-space memory.
 */
#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <stdio.h>

/*
 * the underlying data structure of the wrapper,
 * which should not be accessed directly
 */
struct file_buffer {
	/* the file stream from which to read */
	FILE *in_file;
	/* the size of the file, in bytes */
	size_t file_size;

	/*
	 * the buffer, with a page of space
	 * If there is data, it starts a page size before the real position.
	 */
	unsigned char *buffer;
	/* Is there data in the buffer? */
	int buffered;

	/* the position from which the next byte will be read to the user */
	long virtual_position;
	/* the position from which the next byte will be read from the file */
	long real_position;
};

/* the wrapper used by the API user */
typedef struct file_buffer file_buffer_t;


/*
 * Get the size of the file.
 * buffer:	the file whose size to fetch
 * returns	the number of bytes in the file, ie. the "file_size" field.
 */
inline static size_t get_file_size(file_buffer_t *buffer)
{
	return buffer->file_size;
}

/*
 * Initialize a file buffer from a file stream.
 * to_init:	the buffer to initialize
 * in_file:	the file stream from which to read
 * returns	0 on success,
 *		-1 if the buffer could not be allocated,
 *		   in which case "errno" will be set to ENOMEM,
 *		   or the file's size could not be found,
 *		   in which case "ftell" or "fseek" sets "errno"
 */
int init_file_buffer(file_buffer_t *to_init, FILE *in_file);
/*
 * Initialize a file buffer by opening the specified file.
 * to_open:	the buffer to initialize
 * path:	the path of the file to open
 * returns	0 on success,
 *		-1 if the buffer object could not be initialized,
 *		   in which case "errno" will be set by "init_file_buffer",
 *		   or if the file could not be opened,
 *		   in which case the "fopen" function sets "errno"
 */
int open_file_buffer(file_buffer_t *to_open, const char *path);
/*
 * Destroy a buffer, so that the object can be deallocated,
 * but don't close the file stream.
 * to_destroy:	the buffer to destroy
 */
void destroy_file_buffer(file_buffer_t *to_destroy);
/*
 * Destroy a buffer, so that the object can be deallocated,
 * and close the file stream.
 * to_close:	the buffer to destroy
 */
void close_file_buffer(file_buffer_t *to_close);

/*
 * wrapper to "fseek".
 * Point virtual cursor to the desired location.
 * buffer:	the buffer in which to seek
 * offset:	the relative position to which to point virtual cursor
 * whence:	indicates the base,
 *		relative to which the virtual cursor will move:
 *		SEEK_SET - relative to the beginning of the file
 *		SEEK_CUR - relative to the current position of the file
 *		SEEK_END - relative to the end of the file
 * returns	0 on success,
 *		-1 on failure due to "fseek", which will set "errno",
 *		   or due to invalid value of "whence",
 *		   in which case "errno" is set to "EINVAL".
 *		   or due to an invalid location,
 *		   in which case "errno" is set to "ERANGE".
 */
int fseek_buffer(file_buffer_t *buffer, long offset, int whence);
/*
 * wrapper to "rewind"
 * Rewind the buffer back to the beginning.
 * buffer:	the buffer to rewind
 */
void rewind_buffer(file_buffer_t *buffer);
/*
 * wrapper to "ftell"
 * Return the location of the virtual buffer.
 * buffer:	the buffer whose virtual cursor to find
 * returns	the virtual cursor location
 */
long ftell_buffer(file_buffer_t *buffer);
/*
 * Reads a number of bytes from the buffer.
 * ptr:		the output space
 * size:	the number of bytes to read
 * buffer:	the source buffer
 * returns	number of bytes actually read,
 *		which could be 0
 *		due to error, in which case "errno" will be set,
 *		or the end of the file was reached.
 */
size_t read_buffer_bytes(void *ptr, size_t size, file_buffer_t *buffer);
/*
 * Reads a single byte.
 * buffer:	the buffer from which to read a byte
 * returns	the byte value on a successful read;
 *		EOF, if no byte was read
 *		     due to error, in which case "errno" will be set,
 *		     or the end of the file was reached.
 */
int fgetc_buffer(file_buffer_t *buffer);

#endif /* FILE_BUFFER_H */
