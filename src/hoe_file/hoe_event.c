#include "hoe_event.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_read.h"
#include "file_write.h"
#include "hoe_bytecode.h"
#include "lstring.h"
#include "general.h"

size_t aux_offset = 0;

int add_hoe_chunk(hoe_file *hoe);
uint32_t read_4byte_char(uint8_t *bytecode, size_t *i);
uint8_t read_1byte_char(uint8_t *bytecode, size_t *i);

int is_lstring_char(uint8_t c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || c == '_';
}

void placeholders_check(void *p1, void *p2, char *s)
{
    if (p1 != p2)
    {
        //fprintf(stderr, "ERROR: %p != %p (%s)\n", p1, p2, s);
    }
}

void fill_it_visible_args_event(hoe_event *event)
{
    size_t i = 0;
    size_t found_index = 0;
    char *str1 = "TM_ItSetVisible";
    size_t len1 = strlen(str1);
    event->nb_visible_args = 0;

    while ((found_index = search_in_array(event->bytecode + i,
        event->len_bytecode - i, str1, len1)) != event->len_bytecode - i)
    {
        event->visible_args = realloc(event->visible_args,
            sizeof(ItSet_args) * (event->nb_visible_args + 1));
        event->nb_visible_args++;

        uint32_t type = lsb_32(*(uint32_t *)(event->bytecode + i + found_index + len1 + IT_SET_TYPE_OFFSET));
        uint32_t uid = lsb_32(*(uint32_t *)(event->bytecode + i  + found_index + len1 + IT_SET_UID_OFFSET));
        event->visible_args[event->nb_visible_args - 1].type = type;
        event->visible_args[event->nb_visible_args - 1].uid = uid;
        event->visible_args[event->nb_visible_args - 1].end_of_function_name =
            event->bytecode + i + found_index + len1;
        i += found_index + len1;
    }
}

void fill_it_contained_args_event(hoe_event *event)
{
    size_t i = 0;
    size_t found_index = 0;
    event->nb_contained_args = 0;
    char *str2 = "TM_ItSetContained";
    size_t len2 = strlen(str2);

    while ((found_index = search_in_array(event->bytecode + i,
        event->len_bytecode - i, str2, len2)) != event->len_bytecode - i)
    {
        event->contained_args = realloc(event->contained_args,
            sizeof(ItSet_args) * (event->nb_contained_args + 1));
        event->nb_contained_args++;

        uint32_t type = lsb_32(*(uint32_t *)(event->bytecode + i + found_index + len2 + IT_SET_TYPE_OFFSET));
        uint32_t uid = lsb_32(*(uint32_t *)(event->bytecode + i  + found_index + len2 + IT_SET_UID_OFFSET));
        event->contained_args[event->nb_contained_args - 1].type = type;
        event->contained_args[event->nb_contained_args - 1].uid = uid;
        event->contained_args[event->nb_contained_args - 1].end_of_function_name =
            event->bytecode + i + found_index + len2;
        i += found_index + len2;
    }
}

void fill_it_args(hoe_file *hoe)
{
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        if (hoe->chunks[i].type == HOE_EVENT)
        {
            fill_it_visible_args_event(hoe->chunks[i].content);
            fill_it_contained_args_event(hoe->chunks[i].content);
        }
    }
}

void parse_hoe_var(hoe_var *hoe_var, FILE *file)
{
    size_t r = 0;
    hoe_var->type = (hoe_var_type)read_4byte_msb(file, &r);
    switch (hoe_var->type)
    {
    case HOE_INT:
        hoe_var->ivalue = read_4byte_msb(file, &r);
        hoe_var->fvalue = 0.0;
        break;
    case HOE_FLOAT:
        hoe_var->fvalue = get_first_number_f(file);
        hoe_var->ivalue = 0;
        break;
    default:
        break;
    }
}

int check_lstring(FILE *file, size_t *length)
{
    size_t r = 0;
    uint32_t len = read_4byte_msb(file, &r);
    if (!len)
    {
        fseek(file, -4, SEEK_CUR);
        return 0;
    }

    uint8_t ch;
    for (size_t i = 0; i < len; i++)
    {
        ch = read_1byte(file, &r);
        if (!is_lstring_char(ch))
        {
            fseek(file, -i - 5, SEEK_CUR);
            return 0;
        }
    }

    ch = read_1byte(file, &r);
    fseek(file, -len - 5, SEEK_CUR);
    *length = len;
    return !is_lstring_char(ch);
}

int check_event_content(FILE *file)
{
    float f = get_first_number_f(file);
    fseek(file, -4, SEEK_CUR);
    return f == 4.0;
}

int check_hoe_end(FILE *file)
{
    size_t r = 0;
    uint8_t ch = read_1byte(file, &r);
    if (feof(file))
    {
        return 1;
    }

    fseek(file, -1, SEEK_CUR);
    return 0;
}

int check_instance_content(FILE *file)
{
    size_t r = 0;
    void *place = file->_Placeholder;
    long original_offset = ftell(file);
    /*
    fseek(file, -4, SEEK_CUR);

    uint32_t expected_6 = read_4byte_msb(file, &r);
    if (expected_6 != 0x6)
    {
        placeholders_check(place, file->_Placeholder, "check_instance_content 1");
        return 0;
    }
    */

    fseek(file, 1, SEEK_CUR);
    uint32_t len = read_4byte_msb(file, &r);
    if (len > 0xFFF)
    {
        fseek(file, -5, SEEK_CUR);
        placeholders_check(place, file->_Placeholder, "check_instance_content 2");
        return 0;
    }

    fseek(file, len - 4, SEEK_CUR);
    if (is_file_at_eof(file))
    {
        fseek(file, original_offset, SEEK_SET);
        return 0;
    }

    hoe_chunk_type type = read_4byte_msb(file, &r);
    if (type == HOE_INSTANCE)
    {
        int is_instance = check_instance_content(file);
        fseek(file, original_offset, SEEK_SET);
        return is_instance;
    }
    else if (type == HOE_EVENT)
    {
        int is_event = check_event_content(file);
        fseek(file, original_offset, SEEK_SET);
        return is_event;
    }
    else if (type == HOE_END)
    {
        int is_end = check_hoe_end(file);
        fseek(file, original_offset, SEEK_SET);
        return is_end;
    }

    fseek(file, -len - 5, SEEK_CUR);
    placeholders_check(place, file->_Placeholder, "check_instance_content 3");
    return 0;
}

int file_at_lstring_char(FILE *file)
{
    size_t r = 0;
    uint8_t ch = read_1byte(file, &r);
    fseek(file, -1, SEEK_CUR);
    return is_lstring_char(ch);
}

int file_at_lstrings(FILE *file)
{
    size_t r = 0;
    uint32_t nb_lstrings = read_4byte_msb(file, &r);
    size_t skipped_bytes = 4;
    if (!nb_lstrings || nb_lstrings > 0xFFF)
    {
        fseek(file, -skipped_bytes, SEEK_CUR);
        return 0;
    }

    size_t lstring_len;
    size_t i = 0;
    while (i < nb_lstrings && check_lstring(file, &lstring_len))
    {
        fseek(file, 4 + lstring_len, SEEK_CUR);
        skipped_bytes += 4 + lstring_len;
        i++;
    }

    fseek(file, -skipped_bytes, SEEK_CUR);
    return i == nb_lstrings;
}

int check_end_of_event(FILE *file)
{
    size_t r = 0;
    void *place = file->_Placeholder;
    uint32_t type = read_4byte_msb(file, &r);
    if (type == 0x5)
    {
        int is_event = check_event_content(file);
        fseek(file, -4, SEEK_CUR);
        return is_event;
    }
    else if (type == 0x6)
    {
        int res = check_instance_content(file);
        fseek(file, -4, SEEK_CUR);
        placeholders_check(place, file->_Placeholder, "check_end_of_event 3");
        return res;
    }
    else if (type == 0x4)
    {
        int is_end = check_hoe_end(file);
        fseek(file, -4, SEEK_CUR);
        return is_end;
    }

    fseek(file, -4, SEEK_CUR);
    placeholders_check(place, file->_Placeholder, "check_end_of_event 6");
    return 0;
}

// return 1 if lstrings, 0 if instance
int find_lstrings_or_end_of_event(FILE *file, size_t *offset)
{
    size_t res = 0;
    size_t r = 0;
    uint8_t ch;
    int is_lstrings = 0;

    while (!(is_lstrings = file_at_lstrings(file)) && !check_end_of_event(file))
    {
        fseek(file, 1, SEEK_CUR);
        res++;
    }

    // offset = offset til lstrings (nb_lstrings + lstrings[]) or end of event
    // (start of the next chunk)
    *offset = res;
    fseek(file, -res, SEEK_CUR);
    return is_lstrings;
}

size_t search_end_of_event(FILE *file)
{
    size_t r = 0;
    size_t res = 0;
    uint8_t ch;
    void *place = file->_Placeholder;
    aux_offset = 0;
    while(!check_end_of_event(file))
    {
        fseek(file, 1, SEEK_CUR);
        res++;
        aux_offset++;
        // if (aux_offset > 0x1542 && aux_offset < 0x154F)
        // {
        //     printf("a\n");
        // }
    }

    fseek(file, -res, SEEK_CUR);
    placeholders_check(place, file->_Placeholder, "search_end_of_event");
    return res;
}

int parse_hoe_event(hoe_file *hoe, FILE *file)
{
    if (add_hoe_chunk(hoe))
    {
        fprintf(stderr, "ERROR: couldn't add chunk to hoe\n");
        return 1;
    }

    // TODO: failing case
    hoe_event *event = calloc(1, sizeof(hoe_event));

    size_t r = 0;

    event->magic_number = get_first_number_f(file);
    if (is_file_at_eof(file))
    {
        return HOE_EOF;
    }

    if (read_lstring(file, &event->name))
    {
        fprintf(stderr, "ERROR: couldn't read name lstring\n");
        return 1;
    }

    size_t offset_til_printable = 0;
    int is_lstrings = find_lstrings_or_end_of_event(file, &offset_til_printable);

    event->is_lstrings = is_lstrings;
    if (!is_lstrings)
    {
        // we copy the unknown content, which isn't a sequence of lstrings
        event->uk_content = duplicate_content(file, offset_til_printable);
        event->len_uk_content = offset_til_printable;
        hoe->chunks[hoe->nb_chunks - 1].type = HOE_EVENT;
        hoe->chunks[hoe->nb_chunks - 1].content = event;
        return 0;
    }

    event->nb_uk_ints = offset_til_printable / sizeof(uint32_t);
    // TODO: failing case
    event->uk_ints = calloc(event->nb_uk_ints, sizeof(uint32_t));
    for (uint32_t i = 0; i < event->nb_uk_ints; i++)
    {
        event->uk_ints[i] = read_4byte_msb(file, &r);
    }

    event->nb_lstrings = read_4byte_msb(file, &r);
    event->lstrings = calloc(event->nb_lstrings, sizeof(lstring));
    for (uint32_t i = 0; i < event->nb_lstrings; i++)
    {
        read_lstring(file, event->lstrings + i);
    }

    event->nb_hoe_vars = read_4byte_msb(file, &r);
    event->hoe_vars = calloc(event->nb_hoe_vars, sizeof(hoe_var));
    for (uint32_t i = 0; i < event->nb_hoe_vars; i++)
    {
        parse_hoe_var(event->hoe_vars + i, file);
    }

    event->nb_m1 = read_4byte_msb(file, &r);
    event->m1 = calloc(event->nb_m1, sizeof(uint32_t));
    for (size_t i = 0; i < event->nb_m1; i++)
    {
        event->m1[i] = read_4byte_msb(file, &r);
    }

    size_t offset_til_end_of_event = search_end_of_event(file);
    event->bytecode = duplicate_content(file, offset_til_end_of_event);
    event->len_bytecode = offset_til_end_of_event;

    hoe->chunks[hoe->nb_chunks - 1].type = HOE_EVENT;
    hoe->chunks[hoe->nb_chunks - 1].content = event;
    return 0;
}

void free_hoe_event(hoe_event *event)
{
    free_lstring_content(&event->name);
    free(event->uk_ints);

    if (!event->is_lstrings)
    {
        free(event->uk_content);
        return;
    }

    for (size_t i = 0; i < event->nb_lstrings; i++)
    {
        free_lstring_content(event->lstrings + i);
    }

    free(event->lstrings);

    free(event->hoe_vars);
    free(event->bytecode);
}

int serialize_hoe_event(hoe_event *event, FILE *file)
{
    write_4byte_msb(file, HOE_EVENT);
    write_4byte_float_msb(file, event->magic_number);
    write_4byte_msb(file, event->name.length);
    write_array(file, event->name.content, event->name.length);
    for (size_t i = 0; i < event->nb_uk_ints; i++)
    {
        write_4byte_msb(file, event->uk_ints[i]);
    }

    if (!event->is_lstrings)
    {
        write_array(file, event->uk_content, event->len_uk_content);
        return 0;
    }

    write_4byte_msb(file, event->nb_lstrings);
    for (size_t i = 0; i < event->nb_lstrings; i++)
    {
        write_4byte_msb(file, event->lstrings[i].length);
        write_array(file, event->lstrings[i].content,
            event->lstrings[i].length);
    }

    write_4byte_msb(file, event->nb_hoe_vars);
    for (size_t i = 0; i < event->nb_hoe_vars; i++)
    {
        write_4byte_msb(file, event->hoe_vars[i].type);
        switch(event->hoe_vars[i].type)
        {
        case HOE_INT:
            write_4byte_msb(file, event->hoe_vars[i].ivalue);
            break;
        case HOE_FLOAT:
            write_4byte_float_msb(file, event->hoe_vars[i].fvalue);
            break;
        }
    }

    write_4byte_msb(file, event->nb_m1);
    for (size_t i = 0; i < event->nb_m1; i++)
    {
        write_4byte_msb(file, event->m1[i]);
    }

    for (size_t i = 0; i < event->len_bytecode; i++)
    {
        write_1byte(file, event->bytecode[i]);
    }
    return 0;
}

void print_hoe_event_lstrings(hoe_event *event)
{
    for (size_t i = 0; i < event->nb_lstrings; i++)
    {
        printf("    - ");
        for (size_t j = 0; j < event->lstrings[i].length; j++)
        {
            putchar(event->lstrings[i].content[j]);
        }
        putchar('\n');
    }
}

void print_hoe_event_var(hoe_var *var)
{
    switch (var->type)
    {
    case HOE_INT:

        int is_its = 0;
        int is_item_uid = 0;
        int i;
        for (i = 0; i < NB_ITEMS; i++)
        {
            if (var->ivalue == item_type_short_list[i])
            {
                is_its = 1;
            }
        }
        if (i == NB_ITEMS)
        {
            for (int i = 0; i < NB_ITEM_UIDS; i++)
            {
                if (var->ivalue == item_uid_list[i])
                {
                    is_item_uid = 1;
                }
            }
        }

        if (is_its)
        {
            printf("%s", is_from_its(var->ivalue));
        }
        else if (is_item_uid)
        {
            printf("0x%08X", var->ivalue);
        }
        else
        {
            printf("%d", var->ivalue);
            if (is_some_id(var->ivalue))
                printf(" (0x%08X)", var->ivalue);
        }
        break;
    case HOE_FLOAT:
        printf("%f", var->fvalue);
        break;
    }

}

void print_hoe_event_vars(hoe_event *event)
{
    for (size_t i = 0; i < event->nb_hoe_vars; i++)
    {
        printf("    - ");
        print_hoe_event_var(event->hoe_vars + i);
        printf("\n");
    }
}

void print_hoe_event_bytecode(hoe_event *event)
{
    for (size_t i = 0; i < event->len_bytecode; i++)
    {
        if (i % 16 == 0)
        {
            printf("        ");
        }
        if (is_lstring_char(event->bytecode[i]))
        {
            printf("%2c ", event->bytecode[i]);
        }
        else
        {
            printf("%02X ", event->bytecode[i]);
        }
        if ((i + 1) % 16 == 0 || i == event->len_bytecode - 1)
        {
            putchar('\n');
        }
    }
}

void pretty_print_hoe_event_bytecode_push_old(hoe_event *event, uint8_t *bytecode,
    size_t *i_p)
{
    printf("PUSH ");

    size_t i = *i_p;
    uint32_t integer = lsb_32(*(uint32_t *)(bytecode + i));
    i += 4;
    switch (integer)
    {
    case 1:
        printf("immediate ");
        integer = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        printf("%08X", integer);
        break;
    case 2:
        printf("hoe_var ");
        integer = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        printf("index: %08X value: ", integer);
        print_hoe_event_var(event->hoe_vars + integer);
        break;
    case 4:
        printf("fun ");
        uint32_t len = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        for (size_t j = 0; j < len; j++)
        {
            putchar(bytecode[i + j]);
        }
        i += len;
        printf("\n    (\n");
        size_t nb_args = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        printf("        nb args: %d ", nb_args);
        for (size_t j = 0; j < nb_args; j++)
        {
            size_t byte = bytecode[i];
            printf("%02X (arg)\n        ", byte);
            i++;
            integer = lsb_32(*(uint32_t *)(bytecode + i));
            i += 4;
            if (integer == 0x65)
            {
                pretty_print_hoe_event_bytecode_push_old(event, bytecode, &i);
            }
            else
            {
                printf("%08X", integer);
            }
        }
        printf("\n    )\n");
        break;
    case 8:
        printf("string ");
        uint32_t len_str = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        for (size_t j = 0; j < len_str; j++)
        {
            putchar(bytecode[i + j]);
        }
        i += len_str;
        break;
    default:
        printf("%08X", integer);
        //integer = lsb_32(*(uint32_t *)(bytecode + i));
        break;
    }

    *i_p = i;
    printf("\n");
}

void pretty_print_hoe_event_bytecode_old(hoe_event *event)
{
    uint8_t *bytecode = event->bytecode;
    size_t len_bytecode = event->len_bytecode;

    size_t i = 0;
    while (i < len_bytecode)
    {
        printf("    ");
        uint8_t byte = bytecode[i];
        if (byte)
        {
            printf("%02X\n", byte);
            i++;
            continue;
        }

        uint32_t integer = lsb_32(*(uint32_t *)(bytecode + i));
        i += 4;
        switch (integer)
        {
        case 0x65:
            pretty_print_hoe_event_bytecode_push_old(event, bytecode, &i);
            break;
        default:
            printf("%08X\n", integer);
            break;
        }
    }
}

int pretty_print_hoe_event_bytecode(hoe_event *event)
{
    uint8_t *bytecode = event->bytecode;
    size_t len_bytecode = event->len_bytecode;
    if (!bytecode)
    {
        printf("No bytecode to print!");
        return 0;
    }

    int read_everything_correctly = 1;

    size_t i = 0;
    uint32_t first_number = read_4byte_char(bytecode, &i);
    // if (first_number)
    // {
    //     pretty_print_hoe_event_bytecode_old(event);
    //     return;
    // }
    if (first_number == 1)
    {
        print_hoelang_mask(event, bytecode, &i, 1);

        // we ignore this one cause I don't know what it means
        // first_number = read_4byte_char(bytecode, &i);
    }

    if (print_hoelang_main(event, bytecode, &i, 1))
    {
        return 1;
    }

    if (i >= len_bytecode)
    {
        return 0;
    }

    printf("\nStarting dirty print:\n");

    while (i < len_bytecode)
    {
        printf("%08X\n", read_4byte_char(bytecode, &i));
    }

    return 1;
}

void print_hoe_uk_content(hoe_event *event)
{
    printf("    Uk_content:\n");
    for (size_t i = 0; i < event->len_uk_content; i++)
    {
        if (i % 16 == 0)
        {
            printf("        ");
        }
        printf("%02X ", event->uk_content[i]);
        if ((i + 1) % 16 == 0 || i == event->len_uk_content - 1)
        {
            putchar('\n');
        }
    }

}

void print_hoe_uk_ints(hoe_event *event)
{
    for (size_t i = 0; i < event->nb_uk_ints; i++)
    {
        printf("    - %08X\n", event->uk_ints[i]);
    }
}

void print_hoe_event_it_args(ItSet_args *args, size_t len, char *name,
    hoe_event *event)
{
    printf("    %s args:\n", name);
    for (size_t i = 0; i < len; i++)
    {
        printf("        - Type: %2d (%08X), UID: %2d (%08X)\n", args[i].type,
            event->hoe_vars[args[i].type].ivalue, args[i].uid,
            event->hoe_vars[args[i].uid].ivalue);
    }
}

void print_hoe_m1(uint32_t *m1, size_t nb_m1)
{
    for (size_t i = 0; i < nb_m1; i++)
    {
        printf("    - 0x%08X\n", m1[i]);
    }
}

void print_hoe_event(hoe_event *event)
{
    printf("    Magic number: %f\n", event->magic_number);
    printf("    Name: ");
    for (size_t i = 0; i < event->name.length; i++)
    {
        putchar(event->name.content[i]);
    }
    putchar('\n');

    printf("    Nb_uk_ints: %d\n", event->nb_uk_ints);
    print_hoe_uk_ints(event);

    if (!event->is_lstrings)
    {
        print_hoe_uk_content(event);
        return;
    }

    printf("    Nb_lstrings: %d\n", event->nb_lstrings);
    print_hoe_event_lstrings(event);

    printf("    Nb_hoe_vars: %d\n", event->nb_hoe_vars);
    print_hoe_event_vars(event);

    printf("    Nb_m1: %d\n", event->nb_m1);
    print_hoe_m1(event->m1, event->nb_m1);

    // print_hoe_event_bytecode(event);
    printf("    ```\n");
    pretty_print_hoe_event_bytecode(event);
    printf("    ```\n");

    // if (event->nb_visible_args)
    // {
    //     print_hoe_event_it_args(event->visible_args, event->nb_visible_args, 
    //         "ItSetVisible", event);
    // }

    // if (event->nb_contained_args)
    // {
    //     print_hoe_event_it_args(event->contained_args, event->nb_contained_args,
    //         "ItSetContained", event);
    // }
}