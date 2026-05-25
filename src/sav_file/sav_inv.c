#include "sav_inv.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sav_file.h"
#include "sav_utils.h"

#include "file_read.h"
#include "file_write.h"

/*
** PARSING
*/

int parse_item(sav_inv_item *item, FILE *file)
{
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

    // Finding the Item ID
    item->id = get_item_id_from_loc(it, item_loc);

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
    // TODO: stop if capacity was reached
    while ((error_item = parse_item(&item, file)) == 0 || parse_ammo(sav, file) == 0)
    {
        if (!error_item)
        {
            items[i] = item;
            i++;
        }
    }

    fseek(file, (sav->item_inventory.inv_capacity - i - (sav->handgun_ammo != 0) - (sav->shotgun_ammo != 0)) * 9, SEEK_CUR);
    sav->item_inventory.nb_items = i;

    return 0;
}

int parse_item_inventory_section(sav_file *sav, FILE *file)
{
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

/*
** GETTING
*/
ssize_t index_of_item_with_id(sav_inv *inv, item_id id)
{
    for (size_t i = 0; i < inv->nb_items; i++)
    {
        if (inv->items[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

/*
** MODIFYING
*/

int push_item(sav_inv *inv, sav_inv_item *item)
{
    if (inv->nb_items == inv->inv_capacity)
    {
        return 1;
    }

    inv->items[inv->nb_items] = *item;
    inv->nb_items++;

    return 0;
}

void update_item_at(sav_inv *inv, size_t index, sav_inv_item *item)
{
    inv->items[index] = *item;
}

int update_item_with_id(sav_inv *inv, item_id id, sav_inv_item *item)
{
    ssize_t index = index_of_item_with_id(inv, id);
    if (index == -1)
    {
        return 1;
    }

    update_item_at(inv, (size_t) index, item);
    return 0;
}

// TODO: be careful, this implementation might cause problems
int add_item_to_inv(sav_inv *inv, item_id id, uint8_t amount)
{
    ssize_t index = index_of_item_with_id(inv, id);
    sav_inv_item item = {0};
    item.id = id;
    if (index == -1)
    {
        item.item_loc = get_first_loc_with_id(it, id);
        item.quantity = amount;
        return push_item(inv, &item);
    }

    item = inv->items[index];
    if (item.quantity + amount > 0xFF)
    {
        item.quantity = 0xFF;
    }
    else
    {
        item.quantity += amount;
    }

    update_item_at(inv, index, &item);
    return 0;
}

/*
** SERIALIZING
*/

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

/*
** PRINTING
*/

void print_item(sav_inv_item *item)
{
    item_id id = item->id;
    printf("    ");
    if (sav_config && strcmp(get_config_value(sav_config, "verbose-print"), "true") == 0)
    {
        printf("Item location: ");
        print_item_loc(item->item_loc, id);
    }
    else
    {
        char *name = item_name_from_id(id);
        printf("Item: %s", name);
    }

    putchar('\n');
    printf("    Quantity: %02X\n", item->quantity);
    printf("    Extra info: %08X", item->extra_info);
    if (id == DOCUMENT || id == PHOTO)
    {
        print_extra_info_doc(item->extra_info);
    }
    else if (id == MAP)
    {
        print_extra_info_map(item->extra_info);
    }
    putchar('\n');
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