#include <stdint.h>
#include "crcutil/generic_crc.h"

extern "C" uint32_t crc32(uint32_t prev, void  * data, size_t length);

static constexpr uint32_t CRC_POLY = 0xEDB88320U;

// Note: this implementation does not use CRC acceleration.
// crcutil has an SSE4/CRC-accelerated implementation, but it implements CRC-32C
// with a different polynomial than CRC_POLY.
static crcutil::GenericCrc<uint32_t, uint32_t, size_t, 2> gCrc(CRC_POLY, 32, true);

uint32_t crc32(uint32_t prev, void * data, size_t length){
  // CrcDefault is only fast for x86 machines;
  // otherwise, it defaults to CrcByte, which is not fast.

  // Benchmark: 64 MiB, 100 rounds
  // Intel(R) Core(TM) i7-4510U CPU @ 2.00GHz
  
  return gCrc.CrcDefault(data, length, prev); // 2756 MiB/s
  //return gCrc.CrcBlockword(data, length, prev); // 2858 MiB/s
  //return gCrc.CrcWord(data, length, prev); // 1478 MiB/s
  //return gCrc.CrcByteWord(data, length, prev); // 397 MiB/s
  //return gCrc.CrcByteUnrolled(data, length, prev); // 355 MiB/s
  //return gCrc.CrcByte(data, length, prev); // 356 MiB/s
}
