#include "sav_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int parse_item(sav_inv_item *item, FILE *file)
{
    //
    size_t r = 0;
    uint8_t first_byte = read_1byte(file, &r);
    fseek(file, -1, SEEK_CUR);
    if (first_byte == 0x07 || first_byte == 0x06)
    {
        return 1;
    }

    uint32_t item_loc = read_4byte_lsb(file, &r);
    if (!item_loc)
    {
        return 1;
    }

    uint8_t quantity = read_1byte(file, &r);
    uint32_t extra_info = read_4byte_lsb(file, &r);

    item->item_loc = item_loc;
    item->quantity = quantity;
    item->extra_info = extra_info;

    return 0;
}

int parse_weapon(sav_inv_weapon *weapon, FILE *file)
{
    size_t r = 0;
    weapon->weapon_loc = read_4byte_lsb(file, &r);
    weapon->quantity = read_1byte(file, &r);
    weapon->ammo = read_1byte(file, &r);
    fseek(file, -1, SEEK_CUR);
    weapon->extra_info = read_4byte_lsb(file, &r) & 0xFFFFFF00;
    return 0;
}

int parse_weapon_inventory(sav_inv_weapon *weapons, FILE *file)
{
    size_t i = 0;
    int is_weapon = 0;
    while (i < 28)
    {
        parse_weapon(weapons + i, file);
        i++;
    }

    return 0;
}

int parse_pc_info(sav_pc_info *pc, FILE *file)
{
    //
    fseek(file, 3, SEEK_CUR);
    size_t r = 0;
    pc->door = read_4byte_lsb(file, &r);
    pc->room = read_1byte(file, &r);
    fseek(file, 1, SEEK_CUR);
    pc->x_pos = read_4byte_lsb(file, &r);
    pc->y_pos = read_4byte_lsb(file, &r);
    pc->z_pos = read_4byte_lsb(file, &r);
    pc->rot = read_1byte(file, &r);
    pc->curr_weapon_loc = read_4byte_lsb(file, &r);
    fread(pc->unknown, 1, 29, file);
    pc->health = get_first_number_f_lsb(file);

    parse_weapon_inventory(pc->weapons, file);
    return 0;
}

int parse_pcs(sav_file *sav, FILE *file)
{
    fseek(file, 0x247, SEEK_SET);
    for (size_t i = 0; i < 5; i++)
    {
        //
        parse_pc_info(sav->pcs + i, file);
    }

    return 0;
}

int parse_ammo(sav_file *sav, FILE *file)
{
    size_t r = 0;
    uint8_t first_byte = read_1byte(file, &r);
    if (first_byte == 0x07)
    {
        fseek(file, 4, SEEK_CUR);
        sav->handgun_ammo = read_4byte_lsb(file, &r);
        return 0;
    }
    else if (first_byte == 0x06)
    {
        fseek(file, 4, SEEK_CUR);
        sav->shotgun_ammo = read_4byte_lsb(file, &r);
        return 0;
    }

    fseek(file, -1, SEEK_CUR);
    return 1;
}

int parse_item_inventory(sav_file *sav, FILE *file)
{
    //
    size_t i = 0;
    sav_inv_item item = { 0 };

    memset(sav->items, 0, 60);

    int error_item = 0;
    // TODO: dirty asf
    while ((error_item = parse_item(&item, file)) == 0 || parse_ammo(sav, file) == 0)
    {
        if (!error_item)
        {
            sav->items[i] = item;
            i++;
        }
    }

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

// TODO: finish parsing
// TODO: free everything
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

    fseek(file, 7, SEEK_CUR);

    // this is unnecessary because the function always returns 0, but who knows
    if (parse_item_inventory(sav, file))
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

void print_item(sav_inv_item *item)
{
    printf("    Item location: %06X\n", item->item_loc);
    printf("    Quantity: %02X\n", item->quantity);
    printf("    Extra info: %08X\n", item->extra_info);
}

void print_item_inventory(sav_file *sav)
{
    printf("# Item inventory\n");

    size_t i = 0;
    sav_inv_item *items = sav->items;
    while (items[i].item_loc)
    {
        printf("Printing item %d:\n", i);
        print_item(items + i);
        putchar('\n');
        i++;
    }
}

void print_weapon(sav_inv_weapon *weapon)
{
    printf("        Weapon location: %06X\n", weapon->weapon_loc);
    printf("        Quantity: %02X\n", weapon->quantity);
    printf("        Ammo: %hhu\n", weapon->ammo);
    printf("        Extra info: %08X\n", weapon->extra_info);
}

void print_weapons_inventory(sav_inv_weapon *weapons)
{
    size_t i = 0;
    while (weapons[i].weapon_loc != 0)
    {
        printf("        # Weapon %u\n", i);
        print_weapon(weapons + i);
        putchar('\n');
        i++;
    }
}

void print_pc_info(sav_pc_info *pc)
{
    printf("    Door: %u\n", pc->door);
    printf("    Room: %02X\n", pc->room);
    printf("    X pos: %d\n", pc->x_pos);
    printf("    Y pos: %d\n", pc->y_pos);
    printf("    Z pos: %d\n", pc->z_pos);
    printf("    Rotation: %hhu\n", pc->rot);
    printf("    Current weapon: %06X\n", pc->curr_weapon_loc);

    size_t i = 0;
    while (i < 29)
    {
        if (i % 16 == 0)
        {
            printf("    ");
        }

        printf("%02X ", pc->unknown[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }

        i++;
    }

    putchar('\n');
    printf("    Health: %3.2f\n", pc->health);
    printf("    Weapons inventory:\n");
    print_weapons_inventory(pc->weapons);
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

    print_progress_chunks(sav);
}