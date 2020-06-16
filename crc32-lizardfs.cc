/*
  Copyright 2005-2010 Jakub Kruszona-Zawadzki, Gemius SA, 2013-2014 EditShare, 2013-2015 Skytechnology sp. z o.o..

  This file was part of MooseFS and is part of LizardFS.

  LizardFS is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3.

  LizardFS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LizardFS  If not, see <http://www.gnu.org/licenses/>.
*/

// Looks similar to zlib code.

#include <inttypes.h>
#include <stdlib.h>
#include <cstring>

#undef WORDS_BIGENDIAN
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define WORDS_BIGENDIAN
#endif

extern "C" {
  uint32_t crc32(uint32_t prev, uint8_t * data, size_t length);
  uint32_t crc32_copy(uint32_t prev, uint8_t const * data, size_t length,
					  uint8_t * dest);
  void crc32_init(void);
  void crc32_shutdown(void);
}

static constexpr uint32_t CRC_POLY = 0xEDB88320U;

// Use old code, which supports both big endian and little endian

#define BYTEREV(w) (((w)>>24)+(((w)>>8)&0xff00)+(((w)&0xff00)<<8)+(((w)&0xff)<<24))
static uint32_t crc_table[4][256];

void crc_generate_main_tables(void) {
  uint32_t c,poly,i;

  poly = CRC_POLY;
  for (i=0; i<256; i++) {
    c=i;
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
    c = (c&1) ? (poly^(c>>1)) : (c>>1);
#ifdef WORDS_BIGENDIAN
    crc_table[0][i] = BYTEREV(c);
#else /* little endian */
    crc_table[0][i] = c;
#endif
  }

  for (i=0; i<256; i++) {
#ifdef WORDS_BIGENDIAN
    c = crc_table[0][i];
    c = crc_table[0][(c>>24)]^(c<<8);
    crc_table[1][i] = c;
    c = crc_table[0][(c>>24)]^(c<<8);
    crc_table[2][i] = c;
    c = crc_table[0][(c>>24)]^(c<<8);
    crc_table[3][i] = c;
#else /* little endian */
    c = crc_table[0][i];
    c = crc_table[0][c&0xff]^(c>>8);
    crc_table[1][i] = c;
    c = crc_table[0][c&0xff]^(c>>8);
    crc_table[2][i] = c;
    c = crc_table[0][c&0xff]^(c>>8);
    crc_table[3][i] = c;
#endif
  }
}

inline uint32_t crc_one_byte(uint32_t crc, uint8_t b){
#ifdef WORDS_BIGENDIAN
  return crc_table[0][(crc >> 24) ^ b] ^ (crc << 8);
#else // little-endian
  return crc_table[0][(crc ^ b) & 0xFF] ^ (crc >> 8);
#endif
}

inline uint32_t crc_one_byte_copy(uint32_t crc, uint8_t const * &src, uint8_t *&dest){
  const uint8_t b = *src++;
  *dest++ = b;
  return crc_one_byte(crc, b);
}

inline uint32_t crc_four_bytes(uint32_t crc, uint32_t b4){
  crc ^= b4;
#ifdef WORDS_BIGENDIAN
  return crc_table[0][crc & 0xff] ^
	crc_table[1][(crc >> 8) & 0xff] ^
	crc_table[2][(crc >> 16) & 0xff] ^
	crc_table[3][crc >> 24];
#else // little-endian
  return crc_table[3][crc & 0xff] ^
	crc_table[2][(crc >> 8) & 0xff] ^
	crc_table[1][(crc >> 16) & 0xff] ^
	crc_table[0][crc >> 24];
#endif
}


inline uint32_t crc_four_bytes_copy(uint32_t crc, uint32_t const *&src,
									uint32_t *&dest){
  const uint32_t b4 = *src++;
  *dest++ = b4;
  return crc_four_bytes(crc, b4);
}

inline uint32_t crc_reorder(uint32_t crc){
#ifdef WORDS_BIGENDIAN
  return ~BYTEREV(crc);
#else /* little endian */
  return ~crc;
#endif
}

uint32_t crc32(uint32_t crc, uint8_t * block, size_t leng) {
  const uint32_t *block4;
  crc = crc_reorder(crc);
  while (leng && ((unsigned long)block & 3)) {
	crc = crc_one_byte(crc, *block++);
    leng--;
  }
  block4 = (const uint32_t*)block;
  while (leng>=32) {
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
	crc = crc_four_bytes(crc, *block4++);
    leng-=32;
  }
  while (leng>=4) {
	crc = crc_four_bytes(crc, *block4++);
    leng-=4;
  }
  block = (uint8_t*)block4;
  if (leng) do {
	  crc = crc_one_byte(crc, *block++);
    } while (--leng);
  crc = crc_reorder(crc);
  return crc;
}

uint32_t crc32_copy(uint32_t crc, uint8_t const * block, size_t leng,
					uint8_t * dest) {
  const uint32_t *block4;
  uint32_t *dest4;
  crc = crc_reorder(crc);
  while (leng && ((unsigned long)block & 3)) {
	crc = crc_one_byte_copy(crc, block, dest);
	leng--;
  }
  block4 = (const uint32_t*)block;
  dest4 = (uint32_t*)dest;
  while (leng>=32) {
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	crc = crc_four_bytes_copy(crc, block4, dest4);
	leng-=32;
  }
  while (leng>=4) {
	crc = crc_four_bytes_copy(crc, block4, dest4);
	leng-=4;
  }
  block = (uint8_t*)block4;
  dest = (uint8_t*)dest4;
  if (leng) do {
	  crc = crc_one_byte_copy(crc, block, dest);
	} while (--leng);
  crc = crc_reorder(crc);
  return crc;
}

void crc32_init(void) {
  crc_generate_main_tables();
}

void crc32_shutdown(void) {}
