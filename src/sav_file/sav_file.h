#ifndef SAV_FILE_H
#define SAV_FILE_H

#include <stdint.h>

typedef struct
{
    size_t len;
    uint8_t *content;
} sav_progress_chunk;

typedef struct
{
    uint32_t weapon_loc;
    uint8_t quantity;
    uint8_t ammo;
    uint32_t extra_info;
} sav_inv_weapon;

typedef struct
{
    uint32_t door;
    uint8_t room;
    uint32_t x_pos;
    uint32_t y_pos;
    uint32_t z_pos;
    uint8_t rot;
    uint32_t curr_weapon_loc;
    uint8_t unknown[29];
    float health;

    sav_inv_weapon weapons[28];
} sav_pc_info;

typedef struct
{
    uint32_t item_loc;
    uint8_t quantity;
    uint32_t extra_info;
} sav_inv_item;

typedef struct
{
    uint32_t crc;
    uint32_t index;
    uint8_t room;
    uint8_t nb_saves;
    uint8_t diff_mode;
    uint32_t time;
    uint32_t handgun_ammo;
    uint32_t shotgun_ammo;

    sav_inv_item items[60];

    sav_pc_info pcs[5];

    size_t nb_chunks;
    sav_progress_chunk *progress_chunks;
} sav_file;

sav_file *parse_sav_file(char *path);

void print_sav_file(sav_file *sav);

#endif /* !SAV_FILE_H */