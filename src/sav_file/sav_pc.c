#include "sav_pc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sav_inv.h"
#include "sav_utils.h"

#include "file_read.h"
#include "file_write.h"

#include "it_file.h"

#include "item_id.h"

/*
** PARSING
*/

int parse_weapon(sav_inv_weapon *weapon, FILE *file)
{
    size_t r = 0;
    weapon->weapon_loc = read_4byte_lsb(file, &r);
    weapon->quantity = read_1byte(file, &r);
    weapon->ammo = read_1byte(file, &r);
    fseek(file, -1, SEEK_CUR);
    weapon->extra_info = read_4byte_lsb(file, &r) & 0xFFFFFF00;

    // Finding the Item ID
    weapon->id = get_item_id_from_loc(it, weapon->weapon_loc);

    return 0;
}

int parse_weapon_inventory(sav_pc_info *pc, sav_inv_weapon *weapons,
    uint8_t weapons_capacity, FILE *file)
{
    size_t i = 0;
    size_t nb_weapons = 0;
    int is_weapon = 0;
    while (i < weapons_capacity)
    {
        parse_weapon(weapons + i, file);
        if (weapons[i].weapon_loc != 0)
        {
            nb_weapons++;
        }

        i++;
    }

    pc->nb_weapons = nb_weapons;
    return 0;
}

int parse_pc_info(sav_pc_info *pc, FILE *file)
{
    //
    size_t r = 0;
    pc->pc_info_len = read_2byte_lsb(file, &r);
    pc->weapons_capacity = read_1byte(file, &r);
    pc->door = read_4byte_lsb(file, &r);
    pc->room = read_1byte(file, &r);
    //fseek(file, 1, SEEK_CUR);
    pc->_uk_is_teammate = read_1byte(file, &r);
    pc->x_pos = read_4byte_lsb(file, &r);
    pc->y_pos = read_4byte_lsb(file, &r);
    pc->z_pos = read_4byte_lsb(file, &r);
    pc->rot = read_1byte(file, &r);
    pc->curr_weapon_loc = read_4byte_lsb(file, &r);
    fread(pc->unknown, 1, 29, file);
    pc->health = get_first_number_f_lsb(file);

    sav_inv_weapon *weapons = calloc(pc->weapons_capacity, sizeof(sav_inv_weapon));

    parse_weapon_inventory(pc, weapons, pc->weapons_capacity, file);
    pc->weapons = weapons;
    return 0;
}

int parse_pcs(sav_file *sav, FILE *file)
{
    //fseek(file, 0x247, SEEK_SET);
    for (size_t i = 0; i < 5; i++)
    {
        //
        parse_pc_info(sav->pcs + i, file);
    }

    return 0;
}

/*
** GETTING
*/
ssize_t index_of_weapon_with_id(sav_pc_info *pc, item_id id)
{
    for (size_t i = 0; i < pc->nb_weapons; i++)
    {
        if (pc->weapons[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

/*
** MODIFYING
*/

int push_weapon(sav_pc_info *pc, sav_inv_weapon *weapon)
{
    if (pc->nb_weapons == pc->weapons_capacity)
    {
        return 1;
    }

    pc->weapons[pc->nb_weapons] = *weapon;
    pc->nb_weapons++;

    return 0;
}

void update_weapon_at(sav_pc_info *pc, size_t index, sav_inv_weapon* weapon)
{
    pc->weapons[index] = *weapon;
}

int update_weapon_with_id(sav_pc_info *pc, item_id id, sav_inv_weapon *weapon)
{
    ssize_t index = index_of_weapon_with_id(pc, id);
    if (index == -1)
    {
        return 1;
    }

    update_weapon_at(pc, (size_t) index, weapon);
    return 0;
}

// TODO: be careful, this implementation might cause problems
int add_weapon_to_inv(sav_pc_info *pc, item_id id, uint8_t ammo)
{
    ssize_t index = index_of_weapon_with_id(pc, id);
    sav_inv_weapon weapon = {0};
    weapon.id = id;
    if (index == -1 && 0)
    {
        weapon.weapon_loc = get_first_loc_with_id(it, id);
        weapon.quantity = 1;
        weapon.ammo = ammo;
        return push_weapon(pc, &weapon);
    }

    weapon = pc->weapons[index];
    if (weapon.ammo + ammo > 0xFF)
    {
        weapon.ammo = 0xFF;
    }
    else
    {
        weapon.ammo += ammo;
    }

    update_weapon_at(pc, index, &weapon);
    return 0;
}

/*
** SERIALIZING
*/

void serialize_pc_weapon(sav_inv_weapon *weapon, FILE *file)
{
    write_4byte_lsb(file, weapon->weapon_loc);
    write_1byte(file, weapon->quantity);
    write_4byte_lsb(file, weapon->extra_info);
    fseek(file, -4, SEEK_CUR);
    write_1byte(file, weapon->ammo);
    fseek(file, 3, SEEK_CUR);
}

void serialize_pc(sav_pc_info *pc, FILE *file)
{
    write_2byte_lsb(file, pc->pc_info_len);
    write_1byte(file, pc->weapons_capacity);
    write_4byte_lsb(file, pc->door);
    write_1byte(file, pc->room);
    write_1byte(file, pc->_uk_is_teammate);
    write_4byte_lsb(file, pc->x_pos);
    write_4byte_lsb(file, pc->y_pos);
    write_4byte_lsb(file, pc->z_pos);
    write_1byte(file, pc->rot);
    write_4byte_lsb(file, pc->curr_weapon_loc);
    fwrite(pc->unknown, 1, 29, file);
    write_4byte_float_lsb(file, pc->health);

    for (size_t i = 0; i < pc->weapons_capacity; i++)
    {
        serialize_pc_weapon(pc->weapons + i, file);
    }
}

void serialize_pcs(sav_file *sav, FILE *file)
{
    for (size_t i = 0; i < 5; i++)
    {
        serialize_pc(sav->pcs + i, file);
    }
}

/*
** PRINTING
*/

void print_weapon(sav_inv_weapon *weapon)
{
    item_id id = weapon->id;
    printf("        Weapon location: ");
    print_item_loc(weapon->weapon_loc, id);
    putchar('\n');
    printf("        Quantity: %02X\n", weapon->quantity);
    printf("        Ammo: %hhu\n", weapon->ammo);
    printf("        Extra info: %08X\n", weapon->extra_info);
}

void print_weapons_inventory(sav_inv_weapon *weapons, uint8_t weapons_capacity)
{
    size_t i = 0;
    while (weapons[i].weapon_loc != 0 && i < weapons_capacity)
    {
        printf("        # Weapon %u\n", i);
        print_weapon(weapons + i);
        putchar('\n');
        i++;
    }
}

void print_pc_info(sav_pc_info *pc)
{
    printf("    Door: %u\n", pc->door);
    printf("    Room: %02X\n", pc->room);
    printf("    X pos: %d\n", pc->x_pos);
    printf("    Y pos: %d\n", pc->y_pos);
    printf("    Z pos: %d\n", pc->z_pos);
    printf("    Rotation: %hhu\n", pc->rot);
    item_id id = get_item_id_from_loc(it, pc->curr_weapon_loc);
    printf("    Current weapon: ");
    print_item_loc(pc->curr_weapon_loc, id);
    putchar('\n');

    size_t i = 0;
    while (i < 29)
    {
        if (i % 16 == 0)
        {
            printf("    ");
        }

        printf("%02X ", pc->unknown[i]);

        if ((i + 1) % 16 == 0)
        {
            putchar('\n');
        }

        i++;
    }

    putchar('\n');
    printf("    Health: %3.2f\n", pc->health);
    printf("    Weapons capacity: %zu | nb: %zu\n", pc->weapons_capacity,
        pc->nb_weapons);
    printf("    Weapons inventory:\n");
    print_weapons_inventory(pc->weapons, pc->weapons_capacity);
}