#include "hoe_file.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hoe_event.h"
#include "item.h"
#include "file_read.h"
#include "file_write.h"
#include "lstring.h"
#include "simple_map.h"

int add_hoe_chunk(hoe_file *hoe)
{
    size_t new_size = sizeof(hoe_chunk) * (hoe->nb_chunks + 1);
    hoe_chunk *chunks_new = realloc(hoe->chunks, new_size);
    if (!chunks_new)
    {
        return 1;
    }

    hoe->chunks = chunks_new;

    // we don't -1 the nb_chunks because we haven't increased it yet
    // so we take the last chunk
    hoe->chunks[hoe->nb_chunks].is_unformatted = 0;
    hoe->chunks[hoe->nb_chunks].content_length = 0;
    hoe->chunks[hoe->nb_chunks].content = NULL;

    // we increase it now
    hoe->nb_chunks++;
    return 0;
}

int parse_hoe_imports(hoe_file *hoe, FILE *file)
{
    if (add_hoe_chunk(hoe))
    {
        return 1;
    }

    hoe_imports *imp = calloc(1, sizeof(hoe_imports));
    if (!imp)
    {
        return 1;
    }

    size_t r = 0;
    imp->length = read_4byte_msb(file, &r);
    if (is_file_at_eof(file))
    {
        free(imp);
        return HOE_EOF;
    }
    imp->content = duplicate_content(file, (size_t)imp->length - 4);
    if (!imp->content)
    {
        free(imp);
        // TODO: give better return values?
        return 1;
    }

    hoe->chunks[hoe->nb_chunks - 1].type = HOE_IMPORTS;
    hoe->chunks[hoe->nb_chunks - 1].content = imp;
    return 0;
}

int parse_hoe_collisions(hoe_file *hoe, FILE *file)
{
    if (add_hoe_chunk(hoe))
    {
        return 1;
    }

    hoe_collisions *col = calloc(1, sizeof(hoe_collisions));
    if (!col)
    {
        return 1;
    }

    size_t r = 0;
    col->length = read_4byte_msb(file, &r);
    if (is_file_at_eof(file))
    {
        free(col);
        return HOE_EOF;
    }
    col->content = duplicate_content(file, (size_t)col->length - 4);
    if (!col->content)
    {
        free(col);
        // TODO: give better return values?
        return 1;
    }

    hoe->chunks[hoe->nb_chunks - 1].type = HOE_COLLISIONS;
    hoe->chunks[hoe->nb_chunks - 1].content = col;
    return 0;
}

int parse_hoe_instance(hoe_file *hoe, FILE *file)
{
    if (add_hoe_chunk(hoe))
    {
        return 1;
    }

    // TODO: what if it fails?
    hoe_instance *ins = calloc(1, sizeof(hoe_instance));
    if (!ins)
    {
        return 1;
    }

    size_t r = 0;
    ins->uk_char = read_1byte(file, &r);
    if (is_file_at_eof(file))
    {
        return HOE_EOF;
    }
    ins->length = read_4byte_msb(file, &r);
    if (is_file_at_eof(file))
    {
        return HOE_EOF;
    }
    ins->content = duplicate_content(file, (size_t)ins->length - 4);
    if (!ins->content)
    {
        // TODO: give better return values?
        return 1;
    }

    hoe->chunks[hoe->nb_chunks - 1].type = HOE_INSTANCE;
    hoe->chunks[hoe->nb_chunks - 1].content = ins;
    return 0;
}

hoe_file *parse_hoe_file(uint8_t *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return NULL;
    }

    hoe_file *hoe = calloc(1, sizeof(hoe_file));
    if (!hoe)
    {
        fprintf(stderr, "ERROR: Couldn't calloc hoe\n");
        return NULL;
    }

    hoe->magic_number = get_first_number_f(file);
    hoe->nb_chunks = 0;

    size_t read = 0;

    hoe_chunk_type type;
    while ((type = (hoe_chunk_type)read_4byte_msb(file, &read)) != HOE_END)
    {
        switch (type)
        {
        case HOE_IMPORTS:
            if (parse_hoe_imports(hoe, file))
            {
                fprintf(stderr, "ERROR: imports parsing error\n");
                return NULL;
            }
            break;
        case HOE_COLLISIONS:
            if (parse_hoe_collisions(hoe, file))
            {
                fprintf(stderr, "ERROR: collisions parsing error\n");
                return NULL;
            }
            break;
        case HOE_EVENT:
            int status = 0;
            if ((status = parse_hoe_event(hoe, file)))
            {
                fprintf(stderr, "ERROR: event parsing error: %d\n", status);
                return NULL;
            }
            break;
        case HOE_INSTANCE:
            if (parse_hoe_instance(hoe, file))
            {
                fprintf(stderr, "ERROR: instance parsing error\n");
                return NULL;
            }
            break;
        default:
            break;
        }
    }

    if (!read)
    {
        fprintf(stderr, "ERROR: EOF found before expected\n");
        return NULL;
    }

    // Now we analyze the bytecode to fill the It_args
    fill_it_args(hoe);

    return hoe;
}

void free_hoe_collisions(hoe_collisions *col)
{
    if (!col)
    {
        return;
    }

    free(col->content);
    free(col);
}

void free_hoe_imports(hoe_imports *imp)
{
    if (!imp)
    {
        return;
    }
    free(imp->content);
    free(imp);
}

void free_hoe_instance(hoe_instance *ins)
{
    if (!ins)
    {
        return;
    }

    free(ins->content);
    free(ins);
}

void free_hoe_chunk(hoe_chunk *chunk)
{
    switch (chunk->type)
    {
    case HOE_COLLISIONS:
        free_hoe_collisions(chunk->content);
        break;
    case HOE_IMPORTS:
        free_hoe_imports(chunk->content);
        break;
    case HOE_EVENT:
        free_hoe_event(chunk->content);
        break;
    case HOE_INSTANCE:
        free_hoe_instance(chunk->content);
        break;
    }
}

void free_hoe_file(hoe_file *hoe)
{
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        free_hoe_chunk(hoe->chunks + i);
    }

    free(hoe);
}

/*
** SERIALIZE
*/

int serialize_hoe_imports(hoe_imports *imp, FILE *file)
{
    write_4byte_msb(file, HOE_IMPORTS);
    write_4byte_msb(file, imp->length);
    write_array(file, imp->content, imp->length - 4);
    return 0;
}

int serialize_hoe_collisions(hoe_collisions *col, FILE *file)
{
    write_4byte_msb(file, HOE_COLLISIONS);
    write_4byte_msb(file, col->length);
    write_array(file, col->content, col->length - 4);
    return 0;
}

int serialize_hoe_instance(hoe_instance *ins, FILE *file)
{
    write_4byte_msb(file, HOE_INSTANCE);
    write_1byte(file, ins->uk_char);
    write_4byte_msb(file, ins->length);
    write_array(file, ins->content, ins->length - 4);
    return 0;
}

int serialize_hoe_file(hoe_file *hoe, uint8_t *path)
{
    FILE *file = fopen(path, "wb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't fopen %s\n", path);
        return 1;
    }

    write_4byte_float_msb(file, hoe->magic_number);
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        switch (hoe->chunks[i].type)
        {
        case HOE_IMPORTS:
            serialize_hoe_imports(hoe->chunks[i].content, file);
            break;
        case HOE_COLLISIONS:
            serialize_hoe_collisions(hoe->chunks[i].content, file);
            break;
        case HOE_EVENT:
            serialize_hoe_event(hoe->chunks[i].content, file);
            break;
        case HOE_INSTANCE:
            serialize_hoe_instance(hoe->chunks[i].content, file);
            break;
        }
    }

    write_4byte_msb(file, HOE_END);
    fclose(file);
    return 0;
}

void print_hoe_collisions(hoe_collisions *col)
{
    printf("    Length: %08X\n", col->length);
}

void print_hoe_imports(hoe_imports *imp)
{
    printf("    Length: %08X\n", imp->length);
}

void print_hoe_instance(hoe_instance *ins)
{
    printf("    Uk_char: %02X\n", ins->uk_char);
    printf("    Length: %08X\n", ins->length);
    print_hexdump(ins->content, ins->length - 4, 8);
}

void print_hoe_chunk(hoe_chunk *chunk)
{
    switch (chunk->type)
    {
    case HOE_IMPORTS:
        printf("HOE_IMPORTS:\n");
        print_hoe_imports(chunk->content);
        break;
    case HOE_COLLISIONS:
        printf("HOE_COLLISIONS:\n");
        print_hoe_collisions(chunk->content);
        break;
    case HOE_EVENT:
        printf("HOE_EVENT:\n");
        print_hoe_event(chunk->content);
        break;
    case HOE_INSTANCE:
        printf("HOE_INSTANCE:\n");
        print_hoe_instance(chunk->content);
        break;
    }
}

void print_hoe_file(hoe_file *hoe)
{
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        print_hoe_chunk(hoe->chunks + i);
    }
}

void set_ItSet_args_type(ItSet_args *args, uint32_t type)
{
    args->type = type;
    *(uint32_t*)(args->end_of_function_name + IT_SET_TYPE_OFFSET) =
        lsb_32(type);
}

void set_ItSet_args_uid(ItSet_args *args, uint32_t uid)
{
    args->uid = uid;
    *(uint32_t*)(args->end_of_function_name + IT_SET_UID_OFFSET) =
        lsb_32(uid);
}

int find_key_in_map_with_type_and_uid(simple_map *map, ItSet_args *args, hoe_var *hoe_vars, uint32_t *key)
{
    for (size_t i = 0; i < map->nb_pairs; i++)
    {
        if (hoe_vars[map->pairs[i].key].ivalue == args->type
            && hoe_vars[map->pairs[i].value].ivalue == args->uid)
        {
            *key = map->pairs[i].key;
            return 1;
        }
    }

    return 0;
}

void normalize_hoe_vars_with_args(hoe_event *event, ItSet_args *args,
    size_t nb_args, simple_map *map)
{
    for (size_t i = 0; i < nb_args; i++)
    {
        uint32_t uid = 0;
        if (simple_value_from_key(map, args[i].type, &uid) ==
            SIMPLE_MAP_SUCCESS)
        {
            if (uid != args[i].uid)
            {
                // we check if there's already a key-value pair with the
                // needed values
                ItSet_args to_find =
                {
                    NULL,
                    event->hoe_vars[args[i].type].ivalue,
                    event->hoe_vars[args[i].uid].ivalue
                };
                uint32_t found_key = 0;
                int found = find_key_in_map_with_type_and_uid(map, &to_find,
                    event->hoe_vars, &found_key);

                if (found)
                {
                    set_ItSet_args_type(args + i, found_key);
                    continue;
                }

                // normalize
                // add a new hoe_var
                event->hoe_vars = realloc(event->hoe_vars,
                    sizeof(hoe_var) * (event->nb_hoe_vars + 1));
                event->nb_hoe_vars++;
                event->hoe_vars[event->nb_hoe_vars - 1].type = HOE_INT;
                event->hoe_vars[event->nb_hoe_vars - 1].ivalue =
                    event->hoe_vars[args[i].type].ivalue;
                event->hoe_vars[event->nb_hoe_vars - 1].fvalue = 0.0;

                // change the type to the index of the new hoe_var
                set_ItSet_args_type(args + i, event->nb_hoe_vars - 1);
                simple_add_pair(map, event->nb_hoe_vars - 1, args[i].uid);
            } 
        }
        else
        {
            // associate
            simple_add_pair(map, args[i].type, args[i].uid);
        }
    }
}

void normalize_hoe_vars_event(hoe_event *event)
{
    simple_map map;
    map.nb_pairs = 0;
    map.pairs = NULL;

    normalize_hoe_vars_with_args(event, event->visible_args, event->nb_visible_args, &map);
    normalize_hoe_vars_with_args(event, event->contained_args, event->nb_contained_args, &map);

    /*
    for (size_t i = 0; i < nb_args; i++)
    {
        uint32_t uid = 0;
        if (simple_value_from_key(&map, args[i].type, &uid) ==
            SIMPLE_MAP_SUCCESS)
        {
            if (uid != args[i].uid)
            {
                // normalize
                // add a new hoe_var
                event->hoe_vars = realloc(event->hoe_vars,
                    sizeof(hoe_var) * (event->nb_hoe_vars + 1));
                event->nb_hoe_vars++;
                event->hoe_vars[event->nb_hoe_vars - 1].type = HOE_INT;
                event->hoe_vars[event->nb_hoe_vars - 1].ivalue =
                    event->hoe_vars[args[i].type].ivalue;
                event->hoe_vars[event->nb_hoe_vars - 1].fvalue = 0.0;

                // change the type to the index of the new hoe_var
                set_ItSet_args_type(args + i, event->nb_hoe_vars - 1);
                simple_add_pair(&map, event->nb_hoe_vars - 1, args[i].uid);
            } 
        }
        else
        {
            // associate
            simple_add_pair(&map, args[i].type, args[i].uid);
        }
    }
    */
}

void normalize_hoe_vars(hoe_file *hoe)
{
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        if (hoe->chunks[i].type == HOE_EVENT)
        {
            normalize_hoe_vars_event(hoe->chunks[i].content);
        }
    }
}


void replace_item_hoe_event(hoe_event *event, uint32_t uid, item_type item)
{
    ItSet_args *args = event->visible_args;
    size_t nb_args = event->nb_visible_args;
    for (size_t i = 0; i < nb_args; i++)
    {
        if (args[i].uid != uid)
        {
            continue;
        }
        
        event->hoe_vars[args[i].type].ivalue = (uint32_t)(its_from_it(item));
    }

    args = event->contained_args;
    nb_args = event->nb_contained_args;

    for (size_t i = 0; i < nb_args; i++)
    {
        if (args[i].uid != uid)
        {
            continue;
        }
        
        event->hoe_vars[args[i].type].ivalue = (uint32_t)(its_from_it(item));
    }
}

void replace_item_hoe(hoe_file *hoe, uint32_t uid, item_type item)
{
    for (size_t i = 0; i < hoe->nb_chunks; i++)
    {
        if (hoe->chunks[i].type == HOE_EVENT)
        {
            replace_item_hoe_event(hoe->chunks[i].content, uid, item);
        }
    }
}