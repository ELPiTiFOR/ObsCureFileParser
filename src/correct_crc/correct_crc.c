#include "correct_crc.h"

#include <stddef.h>
#include <stdio.h>

#include "file_read.h"
#include "my_crc.h"

int overwrite_crc(char *path, unsigned int crc)
{
    FILE *file = fopen(path, "rb+");
    if (!file)
    {
        return 1;
    }

    unsigned char towrite[4] = {0};
    void *towrite_v = towrite;
    unsigned int *towrite_u = towrite_v;

    towrite_u[0] = crc;

    //printf("towrite: %x %x %x %x\n", towrite[0], towrite[1], towrite[2], towrite[3]);

    //fseek(file, 0, SEEK_SET);
    fwrite(towrite, 1, 4, file);

    fclose(file);
    return 0;
}
unsigned int crc_from_file(char *path, size_t skip)
{
    size_t written = 0;
    unsigned char *content = str_from_file(path, &written);
    if (!content)
    {
        fprintf(stderr, "ERROR: Couldn't retrieve content from file %s\n", path);
        return 1;
    }

    //printf("CRC = %x | written = %zu\n", crc);
    return crc32(content + skip, written - skip);
}

int correct_sav_checksum(char *path)
{
    unsigned int crc = crc_from_file(path, 4);
    if (overwrite_crc(path, crc))
    {
        fprintf(stderr, "ERROR: Couldn't overwrite crc in file %s\n", path);
        return 1;
    }

    return 0;
}