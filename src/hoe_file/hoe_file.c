#include "hoe_file.h"

#include <stdio.h>
#include <stdlib.h>

#include "file_read.h"

int parse_section(hoe_generic_section *section, FILE *file)
{
    size_t r = 0;
    section->type = read_4byte_msb(file, &r);
    if (r == 0)
    {
        return 1;
    }

    if (section->type == 5)
    {
        return 1;
    }

    size_t len_content = read_4byte_msb(file, &r);
    fseek(file, -4, SEEK_CUR);
    uint8_t *content = calloc(len_content, 1);
    if (!content)
    {
        //
        fprintf(stderr, "ERROR: Couldn't malloc content\n");
        return 1;
    }

    fread(content, 1, len_content, file);

    section->len_content = len_content;
    section->content = content;
    return 0;
}

hoe_file *parse_hoe_file(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Couldn't fopen %s\n", path);
        return NULL;
    }

    hoe_file *hoe = malloc(sizeof(hoe_file));
    if (!hoe)
    {
        fprintf(stderr, "Couldn't malloc hoe %s\n", path);
        fclose(file);
        return NULL;
    }

    size_t r = 0;
    hoe->first_number = read_4byte_msb(file, &r);

    uint32_t type;
    uint32_t len_sections = 0;
    hoe_generic_section *sections = NULL;
    hoe_generic_section sec = { 0 };
    hoe_generic_section *sections = NULL;
    hoe_generic_section sec = { 0 };
    // TODO: to finish
    while (parse_section(&sec, file) == 0 || (sec.type == 5) && parse_event())
    {
        //
        len_sections++;
        hoe_generic_section *sections2 = realloc(sections, len_sections);
        if (!sections2)
        {
            hoe->len_sections = len_sections - 1;
            if (sections)
            {
                hoe->sections = sections;
            }

            free_hoe_file(hoe);
            fclose(file);
            return NULL;
        }

        sections = sections2;
        sections[len_sections - 1] = sec;
    }

    hoe->len_sections = len_sections;
    hoe->sections = sections;
    fclose(file);
    return hoe;
}

void free_hoe_file(hoe_file *hoe)
{
    size_t len_sections = hoe->len_sections;
    hoe_generic_section *p = hoe->sections;
    for (size_t i = 0; i < len_sections; i++)
    {
        free(p[i].content);
    }

    free(p);
    free(hoe);
}

void print_hoe_section(hoe_generic_section *sec)
{
    size_t len_content = sec->len_content;
    uint8_t *content = sec->content;
    printf("    Section type: %d\n", sec->type);
    printf("    Section length: %d\n", len_content);
    printf("    Content:\n");

    for (size_t i = 0; i < len_content; i++)
    {
        //
        if (i % 16 == 0)
        {
            printf("        ");
        }

        printf("%02X ", content[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }
    }

    putchar ('\n');
}

void print_hoe_sections(hoe_file *hoe)
{
    size_t len_sections = hoe->len_sections;
    hoe_generic_section *p = hoe->sections;
    for (size_t i = 0; i < len_sections; i++)
    {
        printf("# Section %d\n", i);
        if (p[i].type == 2)
        {
            printf("    (map of collisions, too big)\n");
        }
        else
        {
            print_hoe_section(p + i);
        }
    }
}

void print_hoe_file(hoe_file *hoe)
{
    printf("# First Number: %08X\n", hoe->first_number);
    print_hoe_sections(hoe);
}