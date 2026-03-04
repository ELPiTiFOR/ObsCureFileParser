#ifndef MAP_ID_H
#define MAP_ID_H

#define NB_MAP_IDS 7

typedef enum
{
    NO_MAP_ID = 0x000000,
    ADMINISTRATION = 'B',
    CLASSROOMS = 'C',
    LIBRARY = 'D',
    REFECTORY = 'E',
    AMPHITHEATRE = 'F',
    DORMITORY = 'I'
} map_id;

extern map_id map_ids[NB_MAP_IDS];
extern char *map_names[NB_MAP_IDS];

char *map_name_from_id(map_id id);

#endif /* !MAP_ID_H */