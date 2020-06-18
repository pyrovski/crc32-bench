#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <linux/if_alg.h>
#include <sys/param.h>
#include <string.h>
#include <strings.h>

// http://www.chronox.de/libkcapi.html
#include <kcapi.h>

static struct kcapi_handle *handle;

void crc32_init(void)
{
    uint32_t flags = 0;

    if (kcapi_md_init(&handle, "crc32", flags)) {
        abort();
    }
    const uint32_t crc = ~0;
    if (kcapi_md_setkey(handle, (uint8_t *) & crc, sizeof(crc))) {
        abort();
    }
}

void crc32_shutdown(void)
{
    kcapi_md_destroy(handle);
}

uint32_t crc32(uint32_t crc, unsigned char *buf, size_t len)
{
    uint32_t crc_out = 0;
    if (kcapi_md_digest
        (handle, buf, len, (uint8_t *) & crc_out,
         sizeof(crc_out)) != sizeof(uint32_t)) {
        abort();
    }
    return crc_out;
}
