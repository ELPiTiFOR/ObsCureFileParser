#include "hoe_event.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file_read.h"
#include "file_write.h"
#include "lstring.h"

size_t aux_offset = 0;

int add_hoe_chunk(hoe_file *hoe);

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

int check_instance_content(FILE *file)
{
    size_t r = 0;
    void *place = file->_Placeholder;
    fseek(file, -4, SEEK_CUR);

    uint32_t expected_6 = read_4byte_msb(file, &r);
    if (expected_6 != 0x6)
    {
        placeholders_check(place, file->_Placeholder, "check_instance_content 1");
        return 0;
    }

    fseek(file, 1, SEEK_CUR);
    uint32_t len = read_4byte_msb(file, &r);
    if (len > 0xFFF)
    {
        fseek(file, -5, SEEK_CUR);
        placeholders_check(place, file->_Placeholder, "check_instance_content 2");
        return 0;
    }
    fseek(file, len - 4, SEEK_CUR);
    hoe_chunk_type type = read_4byte_msb(file, &r);
    fseek(file, -len - 5, SEEK_CUR);
    placeholders_check(place, file->_Placeholder, "check_instance_content 3");
    return type == HOE_END || type == HOE_INSTANCE || type == HOE_EVENT;
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
        if (get_first_number_f(file) == 4.0)
        {
            fseek(file, -8, SEEK_CUR);
            placeholders_check(place, file->_Placeholder, "check_end_of_event 1");
            return 1;
        }

        fseek(file, -8, SEEK_CUR);
        placeholders_check(place, file->_Placeholder, "check_end_of_event 2");
        return 0;
        // size_t lstring_len;
        // int res = check_lstring(file, &lstring_len);
        // fseek(file, -8, SEEK_CUR);
        // return res;
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
        uint8_t ch = read_1byte(file, &r);
        if (feof(file))
        {
            fseek(file, -4, SEEK_CUR);
            placeholders_check(place, file->_Placeholder, "check_end_of_event 4");
            return 1;
        }

        fseek(file, -5, SEEK_CUR);
        placeholders_check(place, file->_Placeholder, "check_end_of_event 5");
        return 0;
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
    fseek(file, event->nb_m1 * 4, SEEK_CUR);
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
        write_4byte_msb(file, 0xFFFFFFFF);
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

void print_hoe_event_vars(hoe_event *event)
{
    for (size_t i = 0; i < event->nb_hoe_vars; i++)
    {
        switch (event->hoe_vars[i].type)
        {
        case HOE_INT:
            printf("    - %d (0x%08X)\n", event->hoe_vars[i].ivalue, event->hoe_vars[i].ivalue);
            break;
        case HOE_FLOAT:
            printf("    - %f\n", event->hoe_vars[i].fvalue);
            break;
        }
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

    print_hoe_event_bytecode(event);
}