#ifndef HOE_FILE_H
#define HOE_FILE_H

#include <stdint.h>

typedef struct
{
    uint32_t type;
    uint32_t len_content;
    uint8_t *content;
} hoe_generic_section;

typedef struct
{
    uint32_t first_number;
    uint32_t len_sections;
    hoe_generic_section *sections;
} hoe_file;

hoe_file *parse_hoe_file(char *path);
void free_hoe_file(hoe_file *hoe);
void print_hoe_file(hoe_file *hoe);

#endif /* !HOE_FILE_H */