#include "file_write.h"

#include "utils.h"

void write_4byte_lsb(FILE *file, uint32_t u)
{
    char buf[4];
    fill_buf_uint32_lsb(u, buf);

    fwrite(buf, 1, 4, file);
}

void write_4byte_msb(FILE *file, uint32_t u)
{
    char buf[4];
    fill_buf_uint32_msb(u, buf);

    fwrite(buf, 1, 4, file);
}

void write_array(FILE *file, uint8_t *arr, uint32_t len)
{
    fwrite(arr, 1, len, file);
}