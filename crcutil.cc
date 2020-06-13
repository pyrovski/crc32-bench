#include <stdint.h>
#include "crcutil/generic_crc.h"

extern "C" unsigned long crc32(uint32_t prev, void  * data, size_t length);

static constexpr int32_t CRC_POLY = 0xEDB88320U;

static crcutil::GenericCrc<uint64_t, uint64_t, uint64_t, 4> gCrc(CRC_POLY, 32, true);

unsigned long crc32(uint32_t prev, void * data, size_t length){
  return gCrc.CrcDefault(data, length, prev);
}
