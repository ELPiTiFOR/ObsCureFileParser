#include "sav_file.h"

#include <stdio.h>
#include <stdlib.h>

#include "file_read.h"

int parse_general_info(sav_file *sav, FILE *file)
{
    // TODO: check each r
    size_t r = 0;
    sav->crc = read_4byte_lsb(file, &r);
    fseek(file, 1, SEEK_CUR);
    sav->index = read_4byte_lsb(file, &r);
    sav->room = read_1byte(file, &r);
    sav->time = read_4byte_lsb(file, &r);
    sav->nb_saves = read_1byte(file, &r);
    fseek(file, 2, SEEK_CUR);
    sav->diff_mode = read_1byte(file, &r);

    return 0;
}

// TODO: finish parsing
sav_file *parse_sav_file(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return NULL;
    }

    sav_file *sav = malloc(sizeof(sav_file));
    if (!sav)
    {
        fprintf(stderr, "ERROR: Couldn't malloc %s\n", path);
        fclose(file);
        return NULL;
    }

    if (parse_general_info(sav, file))
    {
        fprintf(stderr, "ERROR: Couldn't parse general info %s\n", path);
        fclose(file);
        return NULL;
    }
    
    fclose(file);
    return sav;
}

void print_sav_file(sav_file *sav)
{
    printf("# General info\n");
    printf("    Index: %02X\n", sav->index);
    printf("    Room: %02X\n", sav->room);
    printf("    Times saved: %02X\n", sav->nb_saves);
    printf("    Diff mode: %02X\n", sav->diff_mode);
    printf("    Time: %08X\n", sav->time);
    //printf("Handgun ammo: %u\n", sav->handgun_ammo);
    //printf("Shotgun ammo: %u\n", sav->shotgun_ammo);
}