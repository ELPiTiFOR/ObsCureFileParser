#ifndef HOE_FILE_H
#define HOE_FILE_H

#include <stdint.h>

#include "item.h"
#include "lstring.h"

#define HOE_SUCCESS 0
#define HOE_FAIL 1
#define HOE_EOF 2

#define IT_SET_TYPE_OFFSET 0xD
#define IT_SET_UID_OFFSET 0x1A

typedef enum
{
    HOE_COLLISIONS = 0x02,
    HOE_IMPORTS = 0x03,
    HOE_END = 0x04,
    HOE_EVENT = 0x05,
    HOE_INSTANCE = 0x06,
} hoe_chunk_type;

typedef enum
{
    HOE_INT = 0x01,
    HOE_FLOAT = 0x02,
} hoe_var_type;

typedef struct
{
    hoe_var_type type;
    uint32_t ivalue;
    float fvalue;
} hoe_var;

typedef struct
{
    uint32_t length;
    uint8_t *content;
} hoe_collisions;

typedef struct
{
    uint32_t length;
    uint8_t *content;
} hoe_imports;

typedef struct
{
    uint8_t *end_of_function_name;
    uint32_t type;
    uint32_t uid;
} ItSet_args;

typedef struct
{
    float magic_number;

    lstring name;

    uint32_t nb_uk_ints;
    uint32_t *uk_ints;

    // sometimes events don't have lstrings and have a different format (?)
    int is_lstrings;
    size_t len_uk_content;
    uint8_t *uk_content;        // only makes sense if !is_lstrings

    uint32_t nb_lstrings;
    lstring *lstrings;

    uint32_t nb_hoe_vars;
    hoe_var *hoe_vars;

    uint32_t nb_m1;
    uint32_t *m1;

    size_t len_bytecode;
    uint8_t *bytecode;

    size_t nb_visible_args;
    ItSet_args *visible_args;
    size_t nb_contained_args;
    ItSet_args *contained_args;
} hoe_event;

typedef struct
{
    uint8_t uk_char;
    uint32_t length;
    uint8_t *content;
} hoe_instance;

typedef struct
{
    hoe_chunk_type type;
    int is_unformatted;         // tells if content is just a uint8 *
    uint32_t content_length;    // only makes sense if unformatted
    void *content;
} hoe_chunk;

typedef struct
{
    float magic_number;
    uint32_t nb_chunks;
    hoe_chunk *chunks;
} hoe_file;

hoe_file *parse_hoe_file(uint8_t *path);
int serialize_hoe_file(hoe_file *hoe, uint8_t *path);
void print_hoe_file(hoe_file *hoe);
void free_hoe_file(hoe_file *hoe);
void normalize_hoe_vars(hoe_file *hoe);
int is_normalization_hoe_vars_needed(hoe_file *hoe);
void replace_item_hoe(hoe_file *hoe, uint32_t uid, item_type item);

#endif /* !HOE_FILE_H */