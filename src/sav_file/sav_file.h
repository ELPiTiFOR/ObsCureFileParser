#ifndef SAV_FILE_H
#define SAV_FILE_H

#include <stdint.h>

#include "config.h"

#include "it_file.h"

// These are the indexes of each character in the array `pcs` (which comes from
// Playable Characters)
#define STAN_PC_INDEX 0
#define JOSH_PC_INDEX 1
#define KENNY_PC_INDEX 2
#define ASHLEY_PC_INDEX 3
#define SHANNON_PC_INDEX 4

extern it_file *it;
extern config *sav_config;

typedef struct
{
    uint16_t len;
    uint8_t room_id;
    uint8_t *content;
} sav_room_chunk;

typedef struct
{
    uint16_t len;
    uint8_t *content;
} sav_progress_chunk;

typedef struct
{
    item_id id;
    uint32_t weapon_loc;
    uint8_t quantity;
    uint8_t ammo;
    uint32_t extra_info;
} sav_inv_weapon;

typedef struct
{
    uint32_t door;
    uint8_t room;
    uint8_t _uk_is_teammate;
    uint32_t x_pos;
    uint32_t y_pos;
    uint32_t z_pos;
    uint8_t rot;
    uint32_t curr_weapon_loc;
    uint8_t unknown[29];
    float health;

    uint16_t pc_info_len;
    uint8_t weapons_capacity;
    uint8_t nb_weapons;             // actual number of weapons
    sav_inv_weapon *weapons;
} sav_pc_info;

typedef struct
{
    item_id id;
    uint32_t item_loc;
    uint8_t quantity;
    uint32_t extra_info;
} sav_inv_item;

typedef struct
{
    uint16_t inv_len;
    uint8_t inv_capacity;
    uint8_t nb_items;
    sav_inv_item *items;
} sav_inv;

typedef struct
{
    uint32_t crc;
    uint32_t index;
    uint8_t room;
    uint32_t time;
    uint8_t nb_saves;
    uint8_t _uk_sav1[2];
    uint8_t diff_mode;
    uint16_t items_and_pcs_len;

    uint32_t handgun_ammo;
    uint32_t shotgun_ammo;

    sav_inv item_inventory;
    //sav_inv_item items[60];

    sav_pc_info pcs[5];

    size_t nb_chunks;
    sav_progress_chunk *progress_chunks;

    size_t nb_room_chunks;
    sav_room_chunk *room_chunks;
} sav_file;

sav_file *parse_sav_file(char *path);

int serialize_sav_file(sav_file *sav, char *path);

void print_sav_file(sav_file *sav);

int add_item(sav_file *sav, item_id id, uint8_t amount);
int add_weapon(sav_file *sav, size_t pc_index, item_id id, uint8_t ammo);

#endif /* !SAV_FILE_H */