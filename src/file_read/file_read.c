#include "file_read.h"

#include <stdint.h>
#include <stdlib.h>

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

uint16_t read_2byte_lsb(FILE *file, size_t *read)
{
    uint8_t buf[2];
    if ((*read = fread(buf, 1, 2, file)) == 0)
    {
        //fprintf(stderr, "ERROR: Couldn't read 4byte lsb\n");
    }

    void *buf_v = buf;
    uint16_t *buf_i = buf_v;
    return buf_i[0];
}

uint8_t read_1byte(FILE *file, size_t *read)
{
    uint8_t buf[1];
    if ((*read = fread(buf, 1, 1, file)) == 0)
    {
        return 0;
    }

    return buf[0];
}

float get_first_number_f(FILE *file)
{
    uint8_t buf[4];
    size_t r = fread(buf, 1, 4, file);
    if (!r)
    {
        fprintf(stderr, "ERROR: Couldn't get first number\n");
        return -1;
    }

    return buf_to_float(buf, 4);
}

float get_first_number_f_lsb(FILE *file)
{
    uint8_t buf[4];
    size_t r = fread(buf, 1, 4, file);
    if (!r)
    {
        fprintf(stderr, "ERROR: Couldn't get first number\n");
        return -1;
    }

    uint8_t buf_lsb[4];
    for (size_t i = 0; i < 4; i++)
    {
        buf_lsb[i] = buf[3 - i];
    }

    return buf_to_float(buf_lsb, 4);
}

char *str_from_file(char *src, size_t *total_written)
{
    // DON'T DELETE THE "b" IN THE MODE IN fopen() !!!!!!!
    FILE *file = fopen(src, "r+b");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", src);
        return NULL;
    }


    //print_hex_file(file);
    fseek(file, 0, SEEK_SET);

    char *res = calloc(128, 1);
    if (!res)
    {
        fprintf(stderr, "ERROR: Couldn't alloc res");
        fclose(file);
        return NULL;
    }



    size_t _read;
    size_t i = 0;
    while ((_read = fread(res + i, 1, 128 - 1, file)) != 0)
    {
        i += _read;
        if (_read == 127)
        {
            char *res2 = realloc(res, 128 + i + 1);
            if (!res2)
            {
                fprintf(stderr, "ERROR: Couldn't alloc res2");
                fclose(file);
                return NULL;
            }

            res = res2;
        }
    }

    if (total_written)
    {
        *total_written = i;
    }
    res[i] = 0;
    fclose(file);
    return res;
}