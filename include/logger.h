/*
 * Logging and debugging utilities
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <debug_assert.h>

/*
 * logging levels, as represented numerically.
 * Higher values are more severe.
 */
enum log_level {
	FATAL_LEVEL = 5, /* unrecoverable error, and program will stop */
	ERROR_LEVEL = 4, /* undesired execution, but try to continue */
	WARNING_LEVEL = 3, /* unsafe execution */
	INFO_LEVEL = 2, /* execution information */
	/*
	 * failure that should be impossible
	 * --should not appear in production
	 */
	ASSERT_LEVEL = 1,
	/* debug information --should not appear in production */
	DEBUG_LEVEL = 0
};
/* total number of levels */
#define N_LEVELS		(FATAL_LEVEL + 1)

/*
 * Set the default logging level at the production level,
 * ie. above ASSERT_LEVEL, unless DEBUG is defined,
 * in which case, set DISP_LEVEL to DEBUG_LEVEL
 */
#ifndef DISP_LEVEL
#ifdef DEBUG
#define DISP_LEVEL	DEBUG_LEVEL
#else
#define DISP_LEVEL	INFO_LEVEL
#endif /* DEBUG */
#endif /* DISP_LEVEL */

/* Default output is stderr */
#ifndef LOG_FILE
#define LOG_FILE	stderr
#endif

/*
 * Linux console code for setting the text color
 * color_code:	string code for the color
 */
#define SET_COLOR(color_code)	"\x1B[" color_code "m"
/* Linux console code for returning to the default text color */
#define END_COLOR		SET_COLOR("0")

/*
 * Human-readable representation of the logging levels,
 * which will appear in the log output.
 * Texts are indexed by severity level.
 */
static const char *tags[N_LEVELS] = {[DEBUG_LEVEL] =
					SET_COLOR("33") "DEBUG" END_COLOR,
				     [ASSERT_LEVEL] =
					SET_COLOR("36") "ASSERT" END_COLOR,
				     [INFO_LEVEL] =
					SET_COLOR("32") "INFO" END_COLOR,
				     [WARNING_LEVEL] =
					SET_COLOR("33;1") "WARNING" END_COLOR,
				     [ERROR_LEVEL] =
					SET_COLOR("31;1") "ERROR" END_COLOR,
				     [FATAL_LEVEL] =
					SET_COLOR("35;1") "FATAL" END_COLOR};
/*
 * Map the warning level to the string tag
 * level:	the numerical log level that indexes the tag
 * returns	the corresponding log string tag
 */
static inline const char *getTag(size_t level)
{
	debug_assert(level < N_LEVELS);
	return tags[level];
}

/*
 * Print debug tag and current file name and line number,
 * if the level meets the minimum DISP_LEVEL
 */
#define TAG_LOCATION(level) do { \
	if (level >= DISP_LEVEL) { \
		fprintf(LOG_FILE, "[%s] File %s, Line %d\n", getTag(level), \
			__FILE__, __LINE__); \
	} \
} while (0)

/*
 * Writes to log if specified level meets the minimum DISP_LEVEL
 * level:	the log level,
 *		which determines if the message should even be logged,
 *		and how to tag the message
 * fmt:		the format of the message to output,
 *		following printf semantics
 * ...:	the values to plug into the format
 */
__attribute__((format(printf, 2, 3)))
static inline void printlg(enum log_level level, char *fmt, ...)
{
#pragma GCC diagnostic ignored "-Wtype-limits"
	if (level >= DISP_LEVEL) {
#pragma GCC diagnostic warning "-Wtype-limits"
		va_list args;

		fprintf(LOG_FILE, "[%s]: ", getTag(level));

		va_start(args, fmt);
		vfprintf(LOG_FILE, fmt, args);
		va_end(args);
	}
}

/* the number of hexadecimal digits per byte */
#define HEX_DIGITS_PER_BYTE	2
/*
 * Calculates the maximum number of characters
 * needed in a hexadecimal string buffer.
 * size:	the number of raw bytes in memory to show
 * returns	the number of hexadecimal digits, and the null character,
 *		needed to represent the bytes
 */
inline static size_t hex_str_length(size_t size)
{
	return size * HEX_DIGITS_PER_BYTE + 1;
}

/*
 * Convert raw binary data into a hexadecimal string
 * as if it were in big-endian form,
 * similar to the "%x" format argument, but byte-by-byte.
 * out:		the output text buffer
 * in:		the raw, binary input
 * size:	the number of bytes to show
 */
void bytes_to_hex(char *out, const void *in, size_t size);

#endif /* LOGGER_H */
