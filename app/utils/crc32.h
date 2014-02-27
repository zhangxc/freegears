#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

extern const uint32_t crc32_table[256];

/* Return a 32-bit CRC of the contents of the buffer. */
static inline uint32_t
crc32(uint32_t crc, const void *ss, int len)
{
	const unsigned char *s = ss;

	while (--len >= 0)
		crc = crc32_table[(crc ^ *s++) & 0xff] ^ (crc >> 8);

	return crc;
}

#endif
