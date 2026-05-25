#include "hoe_file.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "hoe_event.h"
#include "file_read.h"
#include "lstring.h"

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