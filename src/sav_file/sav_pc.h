#ifndef SAV_PC_H
#define SAV_PC_H

#include <stdio.h>

#include "sav_file.h"

int parse_pcs(sav_file *sav, FILE *file);
int add_weapon_to_inv(sav_pc_info *pc, item_id id, uint8_t ammo);
void serialize_pcs(sav_file *sav, FILE *file);
void print_pc_info(sav_pc_info *pc);

#endif /* !SAV_PC_H */