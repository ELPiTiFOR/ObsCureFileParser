#include "tm_file.h"

#include <stdlib.h>
#include <string.h>

#include "file_write.h"
#include "utils.h"

uint32_t get_first_number(FILE *file)
{
    uint32_t res = 0;
    uint8_t buf[4];
    size_t r = fread(buf, 1, 4, file);
    if (!r)
    {
        fprintf(stderr, "ERROR: Couldn't get first number\n");
        return -1;
    }

    for (size_t i = 0; i < 4; i++)
    {
        res = res << 8;
        res |= buf[i];
    }

    return res;
}

float get_first_number_f(FILE *file)
{
    uint8_t buf[4];
    size_t r = fread(buf, 1, 4, file);
    if (!r)
    {
        fprintf(stderr, "ERROR: Couldn't get first number\n");
        return -1;
    }

    return buf_to_float(buf, 4);
}

long get_len_of_section_tm_file(FILE *file)
{
    /*
    printf("Before printing this buf at offset\n");
    char buf[32];
    fread(buf, 1, 16, file);
    fseek(file, -16, SEEK_CUR);
    print_hex(buf, 20);
    puts("");
    printf("After printing this buf at offset\n");
    */

    // we skip the id of the type of section
    fseek(file, 4, SEEK_CUR);

    // we get the length of the section
    uint8_t len_buf[4];
    size_t read = fread(len_buf, 1, 4, file);
    if (read == 0)
    {
        return 0;
    }

    fseek(file, -8, SEEK_CUR);
    uint32_t len = buf_to_long(len_buf);

    return len;
}

// doesn't fseek back in place
uint8_t *get_content_of_section_tm_file(FILE *file, size_t len)
{
    uint8_t *res = malloc(len);
    if (!res)
    {
        fprintf(stderr, "ERROR: Couldn't alloc content\n");
        return NULL;
    }

    fseek(file, 4, SEEK_CUR);
    size_t r = 0;
    if ((r = fread(res, 1, len, file)) == 0)
    {
        free(res);
        return NULL;
    }

    //fseek(file, -4 - r, SEEK_CUR);
    return res;
}

int get_tm_item_section(FILE *tm_file, tm_item_section *item)
{
    // type
    uint32_t type = get_first_number(tm_file);
    item->type = type;
    item->len_content = get_first_number(tm_file);
    item->item_id = get_first_number(tm_file);
    item->item_location = get_first_number(tm_file);
    item->x_pos = get_first_number(tm_file);
    item->y_pos = get_first_number(tm_file);
    item->z_pos = get_first_number(tm_file);
    /*
    item->x_rot = get_first_number_f(tm_file);
    item->y_rot = get_first_number_f(tm_file);
    item->z_rot = get_first_number_f(tm_file);
    */

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            item->rot_matrix[i][j] = get_first_number_f(tm_file);
        }
    }

    /*
    uint8_t buf[24];
    fread(buf, 1, 24, tm_file);
    for (size_t i = 0; i < 24; i++)
    {
        item->unknown[i] = buf[i];
    }
    */

    item->len_info_diff = get_first_number(tm_file);

    uint8_t *info_diff = malloc(item->len_info_diff);
    if (!info_diff)
    {
        fprintf(stderr, "ERROR: Couldn't malloc for info_diff\n");
        return 1;
    }

    fread(info_diff, 1, item->len_info_diff, tm_file);

    item->info_diff = info_diff;

    return 0;
}

/*
size_t get_offset_of_item_tm_file(FILE *file, uint64_t id_loc)
{
    //size_t old_offset = ftell(file);

    initialize_tm_file(file);
    goto_items_tm_file(file);

    do
    {
        uint64_t curr_item = get_item_id_and_location_tm_file(file);
        if (lsb_from_long(id_loc) == lsb_from_long(curr_item))
        {
            // this is the item we were looking for
            size_t res = ftell(file);
            // printf("Before printing this buf at offset\n");
            // char buf[20];
            // fread(buf, 1, 16, file);
            // print_hex(buf, 20);
            // printf("Before printing this buf at offset\n");

            //fseek(file, old_offset, SEEK_SET);
            return res;
        }
    } while (!goto_next_section_tm_file(file));

    // TODO: fseek() si no se encuentra?
    //fseek(file, old_offset, SEEK_SET);
    return 0;
}
*/


void initialize_tm_file(FILE *file)
{
    fseek(file, 4, SEEK_SET);
}

int goto_next_section_tm_file(FILE *file)
{
    long len = get_len_of_section_tm_file(file);

    fseek(file, len + 4, SEEK_CUR);
    size_t final_offset = ftell(file);

    char buf[2];
    if (!fread(buf, 1, 1, file))
    {
        return 1;
    }

    fseek(file, final_offset, SEEK_SET);
    return 0;
}

int get_id_of_section_tm_file(FILE *file, unsigned char *buf)
{
    int eof = fread(buf, 1, 4, file);
    buf[4] = 0;
    fseek(file, -4, SEEK_CUR);
    return eof == 0;
}

/*
uint64_t get_item_id_and_location_tm_file(FILE *file)
{
    unsigned char data[8];
    int err = get_buf_from_file(file, 8, 15, data);
    if (err)
    {
        return 0;
    }

    return buf_to_long_long(data);
}
*/

void print_tm_item_section(tm_item_section *item)
{
    printf("Type: %d\n", item->type);
    size_t len_content = item->len_content;
    printf("Length of section: %d\n", item->len_content);
    printf("    Item ID: %04X\n", item->item_id);
    printf("    Item location: %06X\n", item->item_location);

    printf("    X pos: %d\n", item->x_pos);
    printf("    Y pos: %d\n", item->y_pos);
    printf("    Z pos: %d\n", item->z_pos);

    /*
    printf("    X rot: %f\n", item->x_rot);
    printf("    Y rot: %f\n", item->y_rot);
    printf("    Z rot: %f\n", item->z_rot);
    */

    for (size_t i = 0; i < 3; i++)
    {
        printf("    [");
        for (size_t j = 0; j < 3; j++)
        {
            printf("%9.3f ", item->rot_matrix[i][j]);
        }
        printf("]\n");
    }

    /*
    for (size_t i = 0; i < 24; i++)
    {
        if (i % 16 == 0)
        {
            printf("    ");
        }

        printf("%02X ", item->unknown[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }
    }
    */

    printf("    Length of Info and Diff: %d\n", item->len_info_diff);
    uint32_t len_info_diff = item->len_info_diff;
    printf("    Info diff: <");
    for (size_t i = 0; i < len_info_diff; i++)
    {
        printf("%c", item->info_diff[i]);
    }

    puts(">");
}

int goto_items_tm_file(FILE *file)
{
    int eof = 0;
    char id[5];
    get_id_of_section_tm_file(file, id);
    while (id[3] != '\x08' && !eof)
    {
        eof = goto_next_section_tm_file(file);
        eof = get_id_of_section_tm_file(file, id);
    }

    return eof;
}

void goto_end_tm_file(FILE *file)
{
    int eof = 0;
    unsigned char id[5];
    get_id_of_section_tm_file(file, id);
    while (id[3] != '\x0A'&& !eof)
    {
        eof = goto_next_section_tm_file(file);
        eof = get_id_of_section_tm_file(file, id);
    }
}

/*
// return 0 if the item is there, 1 otherwise
int check_item(FILE *room_file, uint64_t item)
{
    initialize_tm_file(room_file);
    goto_items_tm_file(room_file);

    int loc = lsb_from_long(item);

    //printf("Entering check_item loop\n");
    do
    {
        uint64_t curr_item = get_item_id_and_location_tm_file(room_file);
        int curr_loc = lsb_from_long(curr_item);

        if (loc == curr_loc)
        {
            return 0;
        }
    } while (!goto_next_section_tm_file(room_file));

    return 1;
}
*/

// TODO: be able to tell if the file is NOT a tm file
tm_file *parse_tm_file(char *path)
{
    FILE *file = fopen(path, "r+b");
    if (!file)
    {
        fprintf(stderr, "Couldn't open tm_file %s\n", path);
        return NULL;
    }

    tm_file *tm = malloc(sizeof(tm_file));
    if (!tm)
    {
        fprintf(stderr, "ERROR: Couldn't alloc tm\n");
        fclose(file);
        return NULL;
    }

    uint32_t fn = get_first_number(file);
    if (fn == -1)
    {
        fclose(file);
        free_tm_file(tm);
        return NULL;
    }

    size_t len_sections = 0;
    size_t len_item_sections = 0;
    char buf_id[5];
    tm_generic_section *sections = NULL;
    tm_item_section *item_sections = NULL;

    while (get_id_of_section_tm_file(file, buf_id) == 0)
    {
        uint32_t type = 0;
        for (size_t i = 0; i < 4; i++)
        {
            type = type << 8;
            type |= buf_id[i];
        }

        if (type == ITEM_SECTION)
        {
            len_item_sections += 1;
            tm_item_section *item_sections2 = realloc(item_sections, len_item_sections * sizeof(tm_item_section));
            if (!item_sections2)
            {
                fclose(file);
                free_tm_file(tm);
                return NULL;
            }

            item_sections = item_sections2;
            tm_item_section *tm_item_section = item_sections + len_item_sections - 1;
            get_tm_item_section(file, tm_item_section);
            continue;
        }

        len_sections += 1;

        tm_generic_section *sections2 = realloc(sections, len_sections * sizeof(tm_generic_section));
        if (!sections2)
        {
            fclose(file);
            free_tm_file(tm);
            return NULL;
        }

        sections = sections2;
        tm_generic_section *tm_section = sections + len_sections - 1;

        tm_section->type = type;
        size_t len_content = get_len_of_section_tm_file(file);
        tm_section->len_content = len_content;
        //printf("file = %p | len_content = %zu\n", file, len_content);
        uint8_t *content = get_content_of_section_tm_file(file, len_content);
        if (!content)
        {
            fclose(file);
            //free_tm_file(tm);
            return NULL;
        }

        tm_section->content = content;
    }

    tm->first_number = fn;
    tm->len_sections = len_sections;
    tm->sections = sections;
    tm->len_item_sections = len_item_sections;
    tm->items = item_sections;
    return tm;
}

size_t search_item_loc_tm(tm_file *tm, uint32_t item_loc)
{
    tm_item_section *items = tm->items;
    size_t len_item_sections = tm->len_item_sections;

    size_t i = 0;
    while (i < len_item_sections && items[i].item_location != item_loc)
    {
        i++;
    }

    return i;
}

int add_item_to_tm(tm_file *tm, tm_item_section *item)
{
    if (search_item_loc_tm(tm, item->item_location) != tm->len_item_sections)
    {
        fprintf(stderr, "ERROR: Can't add item, item_loc %06X is already present\n", item->item_location);
        return 1;
    }

    size_t len_item_sections = tm->len_item_sections;
    len_item_sections++;

    tm_item_section *items = realloc(tm->items, sizeof(tm_item_section) * len_item_sections);
    if (!items)
    {
        fprintf(stderr, "Couldn't realloc tm_item_section *\n");
        return 1;
    }

    // we need to set the 
    uint8_t *info_diff = malloc(item->len_info_diff);
    if (!info_diff)
    {
        // TODO: free something?
        fprintf(stderr, "ERROR: info diff\n");
        return 1;
    }

    for (size_t i = 0; i < item->len_info_diff; i++)
    {
        info_diff[i] = item->info_diff[i];
    }

    item->type = 8;
    item->len_content = 64 + item->len_info_diff;
    item->info_diff = info_diff;

    items[len_item_sections - 1] = *item;

    tm->items = items;
    tm->len_item_sections = len_item_sections;
    return 0;
}

int edit_item_in_tm(tm_file *tm, tm_item_section *item)
{
    size_t index = search_item_loc_tm(tm, item->item_location);
    if (index == tm->len_item_sections)
    {
        fprintf(stderr, "ERROR: Can't edit item, item_loc %06X not found\n", item->item_location);
        return 1;
    }

    if (strcmp(item->info_diff, tm->items[index].info_diff) == 0)
    {
        // if these strings are equal, no need to alloc a new one
        item->info_diff = tm->items[index].info_diff;
    }
    else
    {
        free(tm->items[index].info_diff);
        item->info_diff = duplicate_string(item->info_diff);
    }

    item->type = 8;
    item->len_content = 64 + item->len_info_diff;
    tm->items[index] = *item;
    //print_tm_item_section(tm->items + index);

    return 0;
}


int remove_item_from_tm(tm_file *tm, uint32_t item_loc)
{
    //
    tm_item_section *items = tm->items;
    size_t len_item_sections = tm->len_item_sections;

    size_t i = search_item_loc_tm(tm, item_loc);

    if (i == len_item_sections)
    {
        fprintf(stderr, "ERROR: Couldn't find item with loc %06X\n", item_loc);
        return 1;
    }

    if (i == len_item_sections)
    {
        fprintf(stderr, "ERROR: Couldn't find item with loc %06X\n", item_loc);
        return 1;
    }

    for (size_t j = i; j < len_item_sections - 1; j++)
    {
        items[j] = items[j + 1];
    }

    len_item_sections--;

    items = realloc(tm->items, sizeof(tm_item_section) * len_item_sections);
    if (!items)
    {
        // TODO: Don't realloc yet? vector implementation
        // TODO: LOST DATA
        fprintf(stderr, "Couldn't realloc items (LOST DATA)\n");
        return 1;
    }

    tm->items = items;
    tm->len_item_sections = len_item_sections;

    return 0;
}

int serialize_generic_section(tm_generic_section *section, FILE *file)
{
    write_4byte_msb(file, section->type);
    //write_4byte_msb(file, section->len_content);
    write_array(file, section->content, section->len_content);
}

int serialize_item_section(tm_item_section *item, FILE *file)
{
    write_4byte_msb(file, item->type);
    write_4byte_msb(file, item->len_content);
    write_4byte_msb(file, item->item_id);
    write_4byte_msb(file, item->item_location);
    write_4byte_msb(file, item->x_pos);
    write_4byte_msb(file, item->y_pos);
    write_4byte_msb(file, item->z_pos);
    /*
    write_4byte_msb(file, item->x_rot);
    write_4byte_msb(file, item->y_rot);
    write_4byte_msb(file, item->z_rot);
    */

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            write_4byte_float_msb(file, item->rot_matrix[i][j]);
        }
    }
    //write_array(file, item->unknown, 24);
    write_4byte_msb(file, item->len_info_diff);
    write_array(file, item->info_diff, item->len_info_diff);
}

// TODO: to finish
int serialize_tm_file(tm_file *tm, char *path)
{
    FILE *new_file = fopen(path, "wb");
    if (!new_file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return 1;
    }

    write_4byte_msb(new_file, tm->first_number);
    
    tm_generic_section *sections = tm->sections;
    size_t len_sections = tm->len_sections;

    size_t i = 0;
    while (i < len_sections && sections[i].type < 8)
    {
        // serialize each section between 1 and 7
        serialize_generic_section(sections + i, new_file);
        i++;
    }

    tm_item_section *items = tm->items;
    size_t len_item_sections = tm->len_item_sections;

    for (size_t i = 0; i < len_item_sections; i++)
    {
        // serialize all items (8)
        serialize_item_section(items + i, new_file);
    }

    for (size_t j = i; j < len_sections; j++)
    {
        // serialize each sections > 8
        serialize_generic_section(sections + i, new_file);
    }

    fclose(new_file);
    return 0;
}

void free_tm_file(tm_file *tm)
{
    tm_generic_section *p = tm->sections;
    size_t len_sections = tm->len_sections;
    for (size_t i = 0; i < len_sections; i++)
    {
        free(p[i].content);
    }

    free(tm->sections);
    // TODO: free items
    tm_item_section *p2 = tm->items;
    size_t len_item_sections = tm->len_item_sections;
    for (size_t i = 0; i < len_item_sections; i++)
    {
        free(p2[i].info_diff);
    }
    free(tm->items);
}

void print_tm_generic_section(tm_generic_section *tm_sec)
{
    printf("Type: %d\n", tm_sec->type);
    size_t len_content = tm_sec->len_content;
    printf("Length of section: %d\n", tm_sec->len_content);

    for (size_t i = 0; i < len_content; i++)
    {
        printf("%02X ", tm_sec->content[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }
    }

    putchar('\n');
}


void print_tm_file(tm_file *tm)
{
    printf("# First number: %d\n", tm->first_number);
    size_t len_sections = tm->len_sections;
    size_t len_item_sections = tm->len_item_sections;
    printf("# Sections: \n");
    for (size_t i = 0; i < len_sections; i++)
    {
        print_tm_generic_section(&tm->sections[i]);
    }

    printf("# Items: \n");
    for (size_t i = 0; i < len_item_sections; i++)
    {
        tm_item_section *it = &tm->items[i];
        print_tm_item_section(it);
    }
}
