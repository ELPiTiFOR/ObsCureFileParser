#ifndef SIMPLE_MAP_H
#define SIMPLE_MAP_H

#define SIMPLE_MAP_PAIR_NOT_FOUND 1
#define SIMPLE_MAP_SUCCESS 0

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint32_t key;
    uint32_t value;
} simple_pair;

typedef struct
{
    size_t nb_pairs;
    simple_pair *pairs;
} simple_map;

int simple_value_from_key(simple_map *map, uint32_t key, uint32_t *value);
void simple_add_pair(simple_map *map, uint32_t key, uint32_t value);

#endif /* !SIMPLE_MAP_H */