#ifndef IT_FILE_H
#define IT_FILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "item_id.h"

typedef struct
{
    uint32_t item_id;
    uint32_t item_loc;
    uint32_t extra_info;
    uint32_t multiplier;
    uint32_t diff_mode;
} it_item;

typedef struct
{
    size_t len_items;
    it_item **items;
} it_file;

it_item *parse_it_item(FILE *file);
it_file *parse_it_file(char *path);

int add_item_to_it(it_file *it, it_item *item);
int edit_item_in_it(it_file *it, it_item *item);
int remove_item_from_it(it_file *it, uint32_t item_loc);

int get_item_from_loc(it_file *it, uint32_t item_loc, it_item *item);

int serialize_it_file(it_file *it, char *path);
void print_it_file(it_file *it);
void print_it_item(it_item *item);
void free_it_file(it_file *it);

item_id get_item_id_from_loc(it_file *it, uint32_t item_loc);
uint32_t get_first_loc_with_id(it_file *it, item_id id);

#endif /* !IT_FILE_H */