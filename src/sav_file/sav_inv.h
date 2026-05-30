#ifndef SAV_INV_H
#define SAV_INV_H

#include <stdio.h>

#include "sav_file.h"

int parse_item_inventory_section(sav_file *sav, FILE *file);
int add_item_to_inv(sav_inv *inv, item_type id, uint8_t amount);
void serialize_item_inventory_section(sav_file *sav, FILE *file);
void print_item_inventory(sav_file *sav);

#endif /* !SAV_INV_H */