#include "simple_map.h"

#include <stdlib.h>

int simple_value_from_key(simple_map *map, uint32_t key, uint32_t *value)
{
    if (!map->nb_pairs)
    {
        return SIMPLE_MAP_PAIR_NOT_FOUND;
    }

    for (size_t i = 0; i < map->nb_pairs; i++)
    {
        if (map->pairs[i].key == key)
        {
            *value = map->pairs[i].value;
            return SIMPLE_MAP_SUCCESS;
        }
    }

    return SIMPLE_MAP_PAIR_NOT_FOUND;
}

void simple_add_pair(simple_map *map, uint32_t key, uint32_t value)
{
    map->pairs = realloc(map->pairs, sizeof(simple_pair) * (map->nb_pairs + 1));
    map->nb_pairs++;
    map->pairs[map->nb_pairs - 1].key = key;
    map->pairs[map->nb_pairs - 1].value = value;
}