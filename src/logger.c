#include <logger.h>

#include <inttypes.h>
#include <stdio.h>

void bytes_to_hex(char *out, const void *in, size_t size)
{
	const uint8_t *in_bytes = in;
	size_t byte_i;

	for (byte_i = 0; byte_i < size; byte_i++) {
		snprintf(&out[byte_i * HEX_DIGITS_PER_BYTE],
			 hex_str_length(1), "%02x", in_bytes[byte_i]);
	}

	out[byte_i * HEX_DIGITS_PER_BYTE] = '\0';
}
