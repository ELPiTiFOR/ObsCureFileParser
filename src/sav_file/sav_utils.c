#include "sav_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "item.h"
#include "document_id.h"
#include "rooms_names.h"

void print_room_id(uint8_t room_id)
{
    printf("%02X", room_id);
    char *name = room_name_from_hex_id(room_id);

    if (!name)
    {
        putchar('\n');
        return;
    }

    printf(", %s\n", name);
    free(name);
}

void print_item_loc(uint32_t item_loc, item_type id)
{
    printf("%06X | ", item_loc);
    char *name = is_from_it(id);

    if (id == NOITEM_T || name)
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