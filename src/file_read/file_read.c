#include "file_read.h"

#include <stdint.h>

uint32_t read_4byte_lsb(FILE *file, size_t *read)
{
    uint8_t buf[4];
    if ((*read = fread(buf, 1, 4, file)) == 0)
    {
        //fprintf(stderr, "ERROR: Couldn't read 4byte lsb\n");
    }

    void *buf_v = buf;
    uint32_t *buf_i = buf_v;
    return buf_i[0];
}

uint32_t read_4byte_msb(FILE *file, size_t *read)
{
    uint8_t buf[4];
    if ((*read = fread(buf, 1, 4, file)) == 0)
    {
        //fprintf(stderr, "ERROR: Couldn't read 4byte msb\n");
    }

    uint32_t res = 0;
    for (size_t i = 0; i < 4; i++)
    {
        res = res << 8;
        res |= buf[i];
    }

    return res;
}