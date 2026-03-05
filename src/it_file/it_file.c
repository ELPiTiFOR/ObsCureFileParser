#include "it_file.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file_read.h"
#include "file_write.h"

#include "item_id.h"

it_item *parse_it_item(FILE *file)
{
    size_t read = 0;
    uint32_t item_id = read_4byte_msb(file, &read);
    if (!read)
    {
        //fprintf(stderr, "ERROR: Couldn't parse it_item\n");
        return NULL;
    }

    uint32_t item_loc = read_4byte_msb(file, &read);
    if (!read)
    {
        fprintf(stderr, "ERROR: Couldn't parse it_item\n");
        return NULL;
    }

    uint32_t extra_info = read_4byte_msb(file, &read);
    if (!read)
    {
        fprintf(stderr, "ERROR: Couldn't parse it_item\n");
        return NULL;
    }

    uint32_t multiplier = read_4byte_msb(file, &read);
    if (!read)
    {
        fprintf(stderr, "ERROR: Couldn't parse it_item\n");
        return NULL;
    }

    uint32_t diff_mode = read_4byte_msb(file, &read);
    if (!read)
    {
        fprintf(stderr, "ERROR: Couldn't parse it_item\n");
        return NULL;
    }

    it_item *item = malloc(sizeof(it_item));
    if (!item)
    {
        fprintf(stderr, "ERROR: Couldn't alloc it_item\n");
        return NULL;
    }

    item->item_id = item_id;
    item->item_loc = item_loc;
    item->extra_info = extra_info;
    item->multiplier = multiplier;
    item->diff_mode = diff_mode;

    return item;
}

it_file *parse_it_file(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't read from path %s\n", path);
        return NULL;
    }

    it_file *it = malloc(sizeof(it_file));
    if (!it)
    {
        fprintf(stderr, "ERROR: Couldn't alloc it_file for path %s\n", path);
        fclose(file);
        return NULL;
    }

    size_t len_items = 0;
    it_item **items = NULL;
    it_item *new_item = NULL;

    while ((new_item = parse_it_item(file)))
    {
        len_items++;
        it_item **items2 = realloc(items, sizeof(it_item *) * len_items);
        if (!items2)
        {
            free_it_file(it);
            fprintf(stderr, "ERROR: Couldn't realloc items\n");
            return NULL;
        }

        items = items2;
        items[len_items - 1] = new_item;
    }

    it->len_items = len_items;
    it->items = items;

    fclose(file);
    return it;
}

size_t search_item_loc(it_file *it, uint32_t it_loc)
{
    //
    size_t i = 0;
    size_t len_items = it->len_items;
    it_item **items = it->items;

    while (i < len_items && items[i]->item_loc != it_loc)
    {
        i++;
    }

    return i;
}

size_t search_position_item_loc(it_file *it, uint32_t it_loc)
{
    //
    size_t i = 0;
    size_t len_items = it->len_items;
    it_item **items = it->items;

    while (i < len_items && items[i]->item_loc < it_loc)
    {
        i++;
    }

    return i;
}

it_item *copy_it_item(it_item *item)
{
    it_item *new_item = malloc(sizeof(it_item));
    if (!new_item)
    {
        fprintf(stderr, "ERROR: Couldn't malloc(it_item)\n");
        return NULL;
    }

    *new_item = *item;

    return new_item;
}

// allocates the item
int insert_item_to_it_at(it_file *it, it_item *item, size_t i)
{
    size_t len_items = it->len_items;
    len_items++;
    it_item **items = realloc(it->items, sizeof(it_item *) * len_items);
    if (!items)
    {
        fprintf(stderr, "ERROR: Couldn't realloc\n");
        return 1;
    }

    for (size_t j = i; j < len_items - 1; j++)
    {
        //
        items[len_items - 1 - j + i] = items[len_items - 2 - j + i];
    }

    items[i] = copy_it_item(item);

    it->len_items = len_items;
    it->items = items;
    return 0;
}

// doesn't allocate the item
int add_item_to_it(it_file *it, it_item *item)
{
    size_t index = search_item_loc(it, item->item_loc);
    if (index == it->len_items)
    {
        index = search_position_item_loc(it, item->item_loc);
    }
    else if (it->items[index]->item_loc == item->item_loc)
    {
        fprintf(stderr, "ERROR: Can't add that item, item_loc already present\n");
        return 1;
    }

    return insert_item_to_it_at(it, item, index);
}

int edit_item_in_it(it_file *it, it_item *item)
{
    size_t index = search_item_loc(it, item->item_loc);
    if (index == it->len_items)
    {
        fprintf(stderr, "ERROR: Couldn't find item %06X\n", item->item_loc);
        return 1;
    }

    it->items[index][0] = *item;

    return 0;
}

int remove_item_from_it(it_file *it, uint32_t item_loc)
{
    size_t index = search_item_loc(it, item_loc);
    if (index == it->len_items)
    {
        fprintf(stderr, "ERROR: Couldn't find item\n");
        return 1;
    }

    size_t len_items = it->len_items;
    it_item **items = it->items;

    free(items[index]);

    for (size_t j = index; j < len_items - 1; j++)
    {
        items[j] = items[j + 1];
    }

    len_items--;

    items = realloc(it->items, sizeof(it_item *) * len_items);
    if (!items)
    {
        // TODO: Don't realloc yet? vector implementation
        // TODO: LOST DATA
        fprintf(stderr, "Couldn't realloc items (LOST DATA)\n");
        return 1;
    }

    it->items = items;
    it->len_items = len_items;

    return 0;
}

void serialize_it_item(it_item *item, FILE *file)
{
    write_4byte_msb(file, item->item_id);
    write_4byte_msb(file, item->item_loc);
    write_4byte_msb(file, item->extra_info);
    write_4byte_msb(file, item->multiplier);
    write_4byte_msb(file, item->diff_mode);
}

int serialize_it_file(it_file *it, char *path)
{
    FILE *new_file = fopen(path, "wb");
    if (!new_file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return 1;
    }

    it_item **items = it->items;
    size_t len_items = it->len_items;
    for (size_t i = 0; i < len_items; i++)
    {
        serialize_it_item(items[i], new_file);
    }

    fclose(new_file);
    return 0;
}

int get_item_from_loc(it_file *it, uint32_t item_loc, it_item *item)
{
    size_t len_items = it->len_items;
    it_item **items = it->items;
    for (size_t i = 0; i < len_items; i++)
    {
        if (items[i]->item_loc == item_loc)
        {
            *item = *(items[i]);
            return 0;
        }
    }

    return 1;
}

void free_it_file(it_file *it)
{
    size_t len_items = it->len_items;
    it_item **items = it->items;
    for (size_t i = 0; i < len_items; i++)
    {
        free(items[i]);
    }

    free(items);
    free(it);
}

void print_it_item(it_item *item)
{
    // TODO: show item type name
    printf("    Item ID: %04X\n", item->item_id);
    printf("    Item Location: %06X\n", item->item_loc);
    printf("    Extra Info: %06X\n", item->extra_info);
    printf("    Multiplier: %d\n", item->multiplier);
    printf("    Diff Mode: %02X\n", item->diff_mode);
}

void print_it_file(it_file *it)
{
    printf("# Printing `.it` file\n");
    size_t len_items = it->len_items;
    it_item **items = it->items;
    for (size_t i = 0; i < len_items; i++)
    {
        printf("Item %zu\n", i);
        print_it_item(items[i]);
    }
}

// FROM HERE, FUNCTIONS USE OTHER FILES/LIBS
item_id get_item_id_from_loc(it_file *it, uint32_t item_loc)
{
    //printf("getting item_id from loc %06X\n", item_loc);
    it_item item = {0};
    if (get_item_from_loc(it, item_loc, &item))
    {
        return NO_ITEM_ID;
    }

    return (item_id) item.item_id;
}

uint32_t get_first_loc_with_id(it_file *it, item_id id)
{
    size_t len_items = it->len_items;
    for (size_t i = 0; i < len_items; i++)
    {
        if (it->items[i]->item_id == id)
        {
            return it->items[i]->item_loc;
        }
    }

    return 0x000000;
}