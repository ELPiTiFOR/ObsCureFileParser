#include "sav_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sav_inv.h"
#include "sav_pc.h"
#include "sav_utils.h"

#include "file_read.h"
#include "file_write.h"

#include "it_file.h"

#include "item_id.h"
#include "document_id.h"

it_file *it = NULL;

void setup_it_file(void)
{
    it = parse_it_file("E:\\Fran\\OBSCURE\\Game Data\\Testing ground\\data\\_common\\allitems.it");
}

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
    sav->_uk_sav1[0] = read_1byte(file, &r);
    sav->_uk_sav1[1] = read_1byte(file, &r);
    sav->diff_mode = read_1byte(file, &r);
    fseek(file, 2, SEEK_CUR); // 0x64 00
    sav->items_and_pcs_len = read_2byte_lsb(file, &r);

    return 0;
}

int parse_progress_chunk(sav_file *sav, sav_progress_chunk *chunk, FILE *file)
{
    size_t r = 0;
    chunk->len = read_2byte_lsb(file, &r);
    if (chunk->len == 0)
    {
        // TODO: remove magic numbers
        return 2;
    }

    uint8_t *content = calloc(chunk->len, 1);
    if (!content)
    {
        fprintf(stderr, "ERROR: Couldn't calloc content of chunk\n");
        return 1;
    }

    r = fread(content, 1, chunk->len, file);
    chunk->content = content;

    return 0;
}

int parse_progress_chunks(sav_file *sav, FILE *file)
{
    int status = 0;
    size_t nb_chunks = 0;
    sav_progress_chunk *chunks = NULL;
    sav_progress_chunk new_chunk = { 0 };

    while ((status = parse_progress_chunk(sav, &new_chunk, file)) == 0)
    {
        nb_chunks++;
        sav_progress_chunk *chunks2 = realloc(chunks, sizeof(sav_progress_chunk) * nb_chunks);
        if (!chunks2)
        {
            fprintf(stderr, "ERROR: Couldn't realloc sav_progress_chunk\n");
            return 1;
        }
        chunks = chunks2;
        chunks[nb_chunks - 1] = new_chunk;
    }

    sav->nb_chunks = nb_chunks;
    sav->progress_chunks = chunks;

    return status != 2;
}

// TODO: free everything
sav_file *parse_sav_file(char *path)
{
    setup_it_file();
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

    //fseek(file, 2, SEEK_CUR);

    // this is unnecessary because the function always returns 0, but who knows
    if (parse_item_inventory_section(sav, file))
    {
        fprintf(stderr, "ERROR: Couldn't parse item inventory %s\n", path);
        fclose(file);
        return NULL;
    }

    parse_pcs(sav, file);

    if (parse_progress_chunks(sav, file))
    {
        fprintf(stderr, "ERROR: Couldn't progress chunks %s\n", path);
        fclose(file);
        return NULL;
    }
    
    fclose(file);
    return sav;
}

void serialize_sav_general_info(sav_file *sav, FILE *file)
{
    write_4byte_lsb(file, sav->crc);
    write_1byte(file, 0x06);
    write_4byte_lsb(file, sav->index);
    write_1byte(file, sav->room);
    write_4byte_lsb(file, sav->time);
    write_1byte(file, sav->nb_saves);
    write_1byte(file, sav->_uk_sav1[0]);
    write_1byte(file, sav->_uk_sav1[1]);
    write_1byte(file, sav->diff_mode);
    uint16_t _uk = 0x0064;
    write_2byte_lsb(file, _uk);
    write_2byte_lsb(file, sav->items_and_pcs_len);
}

void serialize_progress_chunk(sav_progress_chunk *chunk, FILE *file)
{
    write_2byte_lsb(file, chunk->len);
    write_array(file, chunk->content, chunk->len);
}

void serialize_progress_chunks(sav_file *sav, FILE *file)
{
    size_t nb_chunks = sav->nb_chunks;
    for (size_t i = 0; i < nb_chunks; i++)
    {
        serialize_progress_chunk(sav->progress_chunks + i, file);
    }
}

int serialize_sav_file(sav_file *sav, char *path)
{
    FILE *new_file = fopen(path, "wb");
    if (!new_file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return 1;
    }

    serialize_sav_general_info(sav, new_file);

    serialize_item_inventory_section(sav, new_file);

    serialize_pcs(sav, new_file);

    serialize_progress_chunks(sav, new_file);

    // filling the rest with zeros
    fseek(new_file, 0, SEEK_END);
    long written = ftell(new_file);
    // TODO: dirty
    long to_write = 0x5800 - written;
    for (long i = 0; i < to_write; i++)
    {
        write_1byte(new_file, 0x00);
    }

    fclose(new_file);
    return 0;
}

void print_general_info(sav_file *sav)
{
    printf("# General info\n");
    printf("    Index: %02X\n", sav->index);
    printf("    Room: %02X\n", sav->room);
    printf("    Times saved: %02X\n", sav->nb_saves);
    printf("    Diff mode: %02X\n", sav->diff_mode);
    printf("    Time: %08X\n", sav->time);

    // the ammo are actually kind of like items, but I consider them
    // a part of the general info
    printf("    Handgun ammo: %u\n", sav->handgun_ammo);
    printf("    Shotgun ammo: %u\n", sav->shotgun_ammo);
    putchar('\n');
}

void print_progress_chunk(sav_progress_chunk *chunk)
{
    size_t len = chunk->len;
    uint8_t *content = chunk->content;

    printf("Chunk length: %d\n", len);
    printf("Chunk content:\n");
    for (size_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            printf("    ");
        }

        printf("%02X ", content[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }
    }

    putchar('\n');
}

void print_progress_chunks(sav_file *sav)
{
    size_t nb_chunks = sav->nb_chunks;
    sav_progress_chunk *chunks = sav->progress_chunks;

    for (size_t i = 0; i < nb_chunks; i++)
    {
        printf("# Chunk number %d\n", i);
        print_progress_chunk(chunks + i);
    }
}

void print_sav_file(sav_file *sav)
{
    print_general_info(sav);
    print_item_inventory(sav);

    for (size_t i = 0; i < 5; i++)
    {
        printf("PC %zu\n", i);
        print_pc_info(sav->pcs + i);
    }

    // TODO: uncomment
    //print_progress_chunks(sav);
}

int add_item(sav_file *sav, item_id id, uint8_t amount)
{
    return add_item_to_inv(&sav->item_inventory, id, amount);
}