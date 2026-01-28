#include "sav_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_read.h"
#include "file_write.h"

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
    fseek(file, -4, SEEK_CUR);
    if (!item_loc)
    {
        return 1;
    }

    fseek(file, 4, SEEK_CUR);
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

int parse_weapon_inventory(sav_inv_weapon *weapons, uint8_t nb_weapons, FILE *file)
{
    size_t i = 0;
    int is_weapon = 0;
    while (i < nb_weapons)
    {
        parse_weapon(weapons + i, file);
        i++;
    }

    return 0;
}

int parse_pc_info(sav_pc_info *pc, FILE *file)
{
    //
    size_t r = 0;
    pc->pc_info_len = read_2byte_lsb(file, &r);
    pc->nb_weapons = read_1byte(file, &r);
    pc->door = read_4byte_lsb(file, &r);
    pc->room = read_1byte(file, &r);
    //fseek(file, 1, SEEK_CUR);
    pc->_uk_is_teammate = read_1byte(file, &r);
    pc->x_pos = read_4byte_lsb(file, &r);
    pc->y_pos = read_4byte_lsb(file, &r);
    pc->z_pos = read_4byte_lsb(file, &r);
    pc->rot = read_1byte(file, &r);
    pc->curr_weapon_loc = read_4byte_lsb(file, &r);
    fread(pc->unknown, 1, 29, file);
    pc->health = get_first_number_f_lsb(file);

    sav_inv_weapon *weapons = calloc(pc->nb_weapons, sizeof(sav_inv_weapon));

    parse_weapon_inventory(weapons, pc->nb_weapons, file);
    pc->weapons = weapons;
    return 0;
}

int parse_pcs(sav_file *sav, FILE *file)
{
    //fseek(file, 0x247, SEEK_SET);
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

int parse_item_inventory(sav_file *sav, sav_inv_item *items, FILE *file)
{
    size_t i = 0;
    sav_inv_item item = { 0 };

    int error_item = 0;
    // TODO: dirty asf
    while ((error_item = parse_item(&item, file)) == 0 || parse_ammo(sav, file) == 0)
    {
        if (!error_item)
        {
            items[i] = item;
            i++;
        }
    }

    fseek(file, (sav->item_inventory.inv_capacity - i - (sav->handgun_ammo != 0) - (sav->shotgun_ammo != 0)) * 9, SEEK_CUR);

    return 0;
}

int parse_item_inventory_section(sav_file *sav, FILE *file)
{
    //
    size_t r;
    sav->handgun_ammo = 0;
    sav->shotgun_ammo = 0;
    sav->item_inventory.inv_len = read_2byte_lsb(file, &r);
    sav->item_inventory.inv_capacity = read_1byte(file, &r);
    sav_inv_item *items = calloc(sav->item_inventory.inv_capacity, sizeof(sav_inv_item));
    if (!items)
    {
        fprintf(stderr, "ERROR: Couldn't calloc items\n");
        return 1;
    }

    if (parse_item_inventory(sav, items, file))
    {
        fprintf(stderr, "ERROR: Couldn't parse_item_inventory\n");
        return 1;
    }

    sav->item_inventory.items = items;

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

void serialize_item(sav_inv_item *item, FILE *file)
{
    write_4byte_lsb(file, item->item_loc);
    write_1byte(file, item->quantity);
    write_4byte_lsb(file, item->extra_info);
}

void serialize_item_inventory_section(sav_file *sav, FILE *file)
{
    sav_inv *inv = &(sav->item_inventory);
    write_2byte_lsb(file, inv->inv_len);
    write_1byte(file, inv->inv_capacity);

    // serializing ammo
    if (sav->shotgun_ammo)
    {
        write_1byte(file, 0x06);
        write_4byte_msb(file, sav->shotgun_ammo);
        fseek(file, -4, SEEK_CUR);
        uint8_t buf[3] = {0};
        write_array(file, buf, 3);
        fseek(file, 1, SEEK_CUR);
        write_4byte_lsb(file, sav->shotgun_ammo);
    }

    if (sav->handgun_ammo)
    {
        write_1byte(file, 0x07);
        write_4byte_msb(file, sav->handgun_ammo);
        fseek(file, -4, SEEK_CUR);
        uint8_t buf[3] = {0};
        write_array(file, buf, 3);
        fseek(file, 1, SEEK_CUR);
        write_4byte_lsb(file, sav->handgun_ammo);
    }

    uint8_t capacity = inv->inv_capacity;
    capacity -= (sav->handgun_ammo != 0) + (sav->shotgun_ammo != 0);
    for (size_t i = 0; i < capacity; i++)
    {
        serialize_item(inv->items + i, file);
    }
}

void serialize_pc_weapon(sav_inv_weapon *weapon, FILE *file)
{
    write_4byte_lsb(file, weapon->weapon_loc);
    write_1byte(file, weapon->quantity);
    write_4byte_lsb(file, weapon->extra_info);
    fseek(file, -4, SEEK_CUR);
    write_1byte(file, weapon->ammo);
    fseek(file, 3, SEEK_CUR);
}

void serialize_pc(sav_pc_info *pc, FILE *file)
{
    write_2byte_lsb(file, pc->pc_info_len);
    write_1byte(file, pc->nb_weapons);
    write_4byte_lsb(file, pc->door);
    write_1byte(file, pc->room);
    write_1byte(file, pc->_uk_is_teammate);
    write_4byte_lsb(file, pc->x_pos);
    write_4byte_lsb(file, pc->y_pos);
    write_4byte_lsb(file, pc->z_pos);
    write_1byte(file, pc->rot);
    write_4byte_lsb(file, pc->curr_weapon_loc);
    fwrite(pc->unknown, 1, 29, file);
    write_4byte_float_lsb(file, pc->health);

    for (size_t i = 0; i < pc->nb_weapons; i++)
    {
        serialize_pc_weapon(pc->weapons + i, file);
    }
}

void serialize_pcs(sav_file *sav, FILE *file)
{
    for (size_t i = 0; i < 5; i++)
    {
        serialize_pc(sav->pcs + i, file);
    }
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
    sav_inv_item *items = sav->item_inventory.items;
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

void print_weapons_inventory(sav_inv_weapon *weapons, uint8_t nb_weapons)
{
    size_t i = 0;
    while (weapons[i].weapon_loc != 0 && i < nb_weapons)
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
    print_weapons_inventory(pc->weapons, pc->nb_weapons);
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