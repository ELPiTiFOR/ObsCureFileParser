#include "sav_utils.h"

#include <stdio.h>
#include <string.h>

#include "item_id.h"
#include "document_id.h"

void print_item_loc(uint32_t item_loc, item_id id)
{
    printf("%06X | ", item_loc);
    char *name = item_name_from_id(id);

    if (id == NO_ITEM_ID || strcmp(name, "NO_ITEM_ID") != 0)
    {
        printf("Item name: %s", name/*, (uint32_t) id*/);
        return;
    }

    printf("Item ID: %04X", (uint32_t) id);
}

void print_extra_info_doc(uint32_t extra_info)
{
    document_id id = (document_id) extra_info;
    char *name = document_name_from_id(id);

    if (strcmp(name, "NO_DOCUMENT_ID") != 0)
    {
        printf(" | Document name: %s", name/*, (uint32_t) id*/);
        return;
    }
}

void print_extra_info_map(uint32_t extra_info)
{
    // TODO: make the letter of the map an enum?
    uint8_t letter = 0;
    letter = extra_info & 0xff;

    printf(" | Map letter: %c", letter);
    return;
}