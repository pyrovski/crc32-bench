#include <stdint.h>
#include "crcutil/generic_crc.h"

extern "C" uint32_t crc32(uint32_t prev, void  * data, size_t length);

static constexpr uint32_t CRC_POLY = 0xEDB88320U;

static crcutil::GenericCrc<uint32_t, uint32_t, size_t, 2> gCrc(CRC_POLY, 32, true);

uint32_t crc32(uint32_t prev, void * data, size_t length){
  return gCrc.CrcDefault(data, length, prev);
}
