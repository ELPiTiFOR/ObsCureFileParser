#include "map_id.h"

#include <stddef.h>
#include <stdio.h>

map_id map_ids[] =
{
    NO_MAP_ID,
    ADMINISTRATION,
    CLASSROOMS,
    LIBRARY,
    REFECTORY,
    AMPHITHEATRE,
    DORMITORY
};

char *map_names[] =
{
    "NO_MAP_ID",
    "ADMINISTRATION",
    "CLASSROOMS",
    "LIBRARY",
    "REFECTORY",
    "AMPHITHEATRE",
    "DORMITORY"
};

char *map_name_from_id(map_id id)
{
    // First we look for the ID in the array of map_id
    // TODO: implement dichotomy search
    for (size_t i = 0; i < NB_MAP_IDS; i++)
    {
        //printf("Looking for %08X, iteration %zu\n", id, i);
        if (map_ids[i] == id)
        {
            return map_names[i];
        }
    }
    
    // If not found, return "NO_map_ID"
    return map_names[0];
}