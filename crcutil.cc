#include <stdint.h>
#include "crcutil/generic_crc.h"

extern "C" uint32_t crc32(uint32_t prev, void  * data, size_t length);

static constexpr uint32_t CRC_POLY = 0xEDB88320U;

// Note: this implementation does not use CRC acceleration.
// crcutil has an SSE4/CRC-accelerated implementation, but it implements CRC-32C
// with a different polynomial than CRC_POLY.
static crcutil::GenericCrc<uint32_t, uint32_t, size_t, 2> gCrc(CRC_POLY, 32, true);

uint32_t crc32(uint32_t prev, void * data, size_t length){
  return gCrc.CrcDefault(data, length, prev);
}
